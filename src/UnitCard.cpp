#include "UnitCard.h"
#include <iostream>

int UnitCard::totalCreated   = 0;
int UnitCard::totalDestroyed = 0;

// Constructor simplu: delegă constructorului complet cu un AttackEffects gol.
UnitCard::UnitCard(const std::string& name, unsigned manaCost, int attack, int hp,
                   const std::string& attackEffectSummary)
    : UnitCard(name, manaCost, attack, hp, AttackEffects{}, attackEffectSummary) {
}

UnitCard::UnitCard(const std::string& name, unsigned manaCost, int attack, int hp,
                   const AttackEffects& effects, const std::string& attackEffectSummary)
    : Card(name, manaCost),
      onAttackBefore(effects.onBefore),
      onAttackAfter(effects.onAfter),
      attackEffectSummary(attackEffectSummary),
      attack(attack),
      hp(hp),
      isDead(false),
      hasAttackedThisTurn(false) {
    ++totalCreated;
}

// Constructor de copiere: copiază toți membrii și incrementează contorul.
UnitCard::UnitCard(const UnitCard& other)
    : Card(other), // apelează constructorul de copiere al clasei de bază (Card)
      onAttackBefore(other.onAttackBefore),
      onAttackAfter(other.onAttackAfter),
      attackEffectSummary(other.attackEffectSummary),
      attack(other.attack),
      hp(other.hp),
      isDead(other.isDead),
      hasAttackedThisTurn(other.hasAttackedThisTurn) {
    ++totalCreated;
}

// Destructorul: dacă unitatea nu a murit în joc, o numărăm ca distrusă acum.
// Acest lucru se întâmplă când jocul se termină și vectorii sunt eliberați din memorie.
UnitCard::~UnitCard() {
    if (!isDead) {
        ++totalDestroyed;
    }
}

void swap(UnitCard& first, UnitCard& second) {
    using std::swap;
    // static_cast<Card&> aplică swap-ul și asupra părții moștenite din Card.
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.onAttackBefore,      second.onAttackBefore);
    swap(first.onAttackAfter,       second.onAttackAfter);
    swap(first.attackEffectSummary, second.attackEffectSummary);
    swap(first.attack,              second.attack);
    swap(first.hp,                  second.hp);
    swap(first.isDead,              second.isDead);
    swap(first.hasAttackedThisTurn, second.hasAttackedThisTurn);
}

// Operator de atribuire prin copy-and-swap: 'other' este primit prin valoare (se copiază),
// apoi conținutul este schimbat cu *this. La ieșire, copia temporară este distrusă automat.
UnitCard& UnitCard::operator=(UnitCard other) {
    swap(*this, other);
    return *this;
}

void UnitCard::onPlayEffect() {
    std::cout << name << " has entered the board\n";
}

void UnitCard::onDeath() {
    if (isDead) {
        return; // Protecție: logica de moarte nu se execută de două ori.
    }
    isDead = true;
    ++totalDestroyed;
    hp = 0;
    std::cout << name << " has been destroyed and disappeared from the board\n";
}

// make_unique<UnitCard>(*this) apelează constructorul de copiere definit mai sus.
std::unique_ptr<Card> UnitCard::clone() const {
    return std::make_unique<UnitCard>(*this);
}

void UnitCard::displayDetails(std::ostream& os) const {
    os << " | ATK: " << attack << " | HP: " << hp;
    if (hasAttackedThisTurn) {
        os << " [exhausted]";
    }
    if (!attackEffectSummary.empty()) {
        os << " | Effect: " << attackEffectSummary;
    }
}

void UnitCard::onAttack(UnitCard& targetCard) {
    if (!isAlive() || !targetCard.isAlive() || hasAttackedThisTurn) {
        return;
    }

    if (onAttackBefore) {
        onAttackBefore(*this, targetCard);
    }

    targetCard.updateHp(-attack);
    std::cout << name << " attacks " << targetCard.getName() << " for " << attack << " damage!\n";
    hasAttackedThisTurn = true; // Unitatea nu mai poate ataca în acest tur.

    if (onAttackAfter) {
        onAttackAfter(*this, targetCard);
    }
}

void UnitCard::updateHp(int amount) {
    if (isDead || amount == 0) return;
    hp += amount;
    if (hp <= 0) {
        onDeath(); // HP-ul a ajuns la 0 sau sub: declanșează moartea.
    }
}

void UnitCard::clearAttackEffects() {
    onAttackBefore      = nullptr;
    onAttackAfter       = nullptr;
    attackEffectSummary = "";
}
