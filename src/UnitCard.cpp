#include "UnitCard.h"
#include "Exceptions.h"
#include <iostream>
#include <limits>

int UnitCard::totalCreated   = 0;
int UnitCard::totalDestroyed = 0;

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
    if (attack <= 0) {
        throw InvalidCardStatException(name, "attack", attack);
    }
    if (hp <= 0) {
        throw InvalidCardStatException(name, "HP", hp);
    }
    ++totalCreated;
}

UnitCard::UnitCard(const UnitCard& other)
    : Card(other),
      onAttackBefore(other.onAttackBefore),
      onAttackAfter(other.onAttackAfter),
      attack(other.attack),
      hp(other.hp),
      isDead(other.isDead),
      hasAttackedThisTurn(other.hasAttackedThisTurn) {
    ++totalCreated;
}

UnitCard::~UnitCard() {
    if (!isDead) {
        ++totalDestroyed;
    }
}

void swap(UnitCard& first, UnitCard& second) {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.onAttackBefore,      second.onAttackBefore);
    swap(first.onAttackAfter,       second.onAttackAfter);
    swap(first.attack,              second.attack);
    swap(first.hp,                  second.hp);
    swap(first.isDead,              second.isDead);
    swap(first.hasAttackedThisTurn, second.hasAttackedThisTurn);
}

UnitCard& UnitCard::operator=(UnitCard other) {
    swap(*this, other);
    return *this;
}

void UnitCard::onPlayEffect() {
    std::cout << name << " has entered the board\n";
}

void UnitCard::onDeath() {
    if (isDead) {
        return;
    }
    isDead = true;
    ++totalDestroyed;
    hp = 0;
    std::cout << name << " has been destroyed and disappeared from the board\n";
}

std::unique_ptr<Card> UnitCard::clone() const {
    return std::make_unique<UnitCard>(*this);
}

void UnitCard::displayDetails(std::ostream& os) const {
    os << " | ATK: " << attack << " | HP: " << hp;
    if (hasAttackedThisTurn) {
        os << " [exhausted]";
    }
    Card::displayDetails(os);
}

void UnitCard::onAttack(UnitCard& targetCard) {
    if (!isAlive() || !targetCard.isAlive() || hasAttackedThisTurn) {
        return;
    }

    hasAttackedThisTurn = true;
    if (onAttackBefore) {
        onAttackBefore(*this, targetCard);
    }
    if (!isAlive() || !targetCard.isAlive()) return;

    const int damageToTarget = attack;
    const int counterDamage = targetCard.attack;
    targetCard.updateHp(-damageToTarget);
    updateHp(-counterDamage);
    std::cout << name << " attacks " << targetCard.getName() << " for "
              << damageToTarget << " damage and takes " << counterDamage << " damage!\n";

    if (onAttackAfter) {
        onAttackAfter(*this, targetCard);
    }
}

void UnitCard::updateHp(int amount) {
    if (isDead || amount == 0) return;
    const int maximum = std::numeric_limits<int>::max();
    if (amount > 0 && hp > maximum - amount) {
        hp = maximum;
    } else {
        hp += amount;
    }
    if (hp <= 0) {
        onDeath();
    }
}

void UnitCard::clearAttackEffects() {
    onAttackBefore      = nullptr;
    onAttackAfter       = nullptr;
    effectSummary = "";
}
