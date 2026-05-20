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
    : Card(name, manaCost, attackEffectSummary),
      onAttackBefore(effects.onBefore),
      onAttackAfter(effects.onAfter),
      attack(attack),
      hp(hp),
      isDead(false),
      hasAttackedThisTurn(false) {
    ++totalCreated;
}

// Constructorul de copiere: este apelat atunci cand cream o carte noua ca o copie a uneia existente.
UnitCard::UnitCard(const UnitCard& other)
    : Card(other), // Apeleaza mai intai constructorul de copiere al clasei parinte (Card)
      onAttackBefore(other.onAttackBefore),
      onAttackAfter(other.onAttackAfter),
      attack(other.attack),
      hp(other.hp),
      isDead(other.isDead),
      hasAttackedThisTurn(other.hasAttackedThisTurn) {
    ++totalCreated; // Incrementam numarul total de unitati create in memorie
}

// Destructorul: apelat automat cand cartea este distrusa definitiv din memorie.
UnitCard::~UnitCard() {
    if (!isDead) {
        ++totalDestroyed; // Daca unitatea era inca vie pe tabla, o numaram acum ca distrusa
    }
}

// Functie de intrajutorare care schimba toate datele intre doua unitati.
void swap(UnitCard& first, UnitCard& second) {
    using std::swap;
    // static_cast<Card&> ne permite sa schimbam datele mostenite din clasa parinte (Card)
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.onAttackBefore,      second.onAttackBefore);
    swap(first.onAttackAfter,       second.onAttackAfter);
    swap(first.attack,              second.attack);
    swap(first.hp,                  second.hp);
    swap(first.isDead,              second.isDead);
    swap(first.hasAttackedThisTurn, second.hasAttackedThisTurn);
}

// Operatorul de atribuire (=): foloseste tehnica extrem de sigura "Copy-and-Swap".
// Pasul 1: Parametrul 'other' este primit prin valoare, deci compilatorul face automat o copie a sa.
// Pasul 2: Facem 'swap' intre datele noastre curent si copia 'other'.
// Pasul 3: Cand functia se termina, copia (care acum are datele noastre vechi) este stearsa automat din memorie.
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
    // Apelăm și afișarea efectului din clasa de bază.
    Card::displayDetails(os);
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
    effectSummary = "";
}
