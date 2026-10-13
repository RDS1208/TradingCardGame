#include "StructureCard.h"
#include "Exceptions.h"
#include "Game.h"
#include <iostream>
#include <limits>

StructureCard::StructureCard(const std::string& name, unsigned manaCost, int hp,
                             const std::string& effectSummary)
    : StructureCard(name, manaCost, hp, nullptr, effectSummary) {
}

StructureCard::StructureCard(const std::string& name, unsigned manaCost, int hp,
                             TurnChangeEffect effect, const std::string& effectSummary)
    : Card(name, manaCost, effectSummary),
      effect(effect),
      hp(hp),
      isDead(false) {
    if (hp <= 0) {
        throw InvalidCardStatException(name, "HP", hp);
    }
}

StructureCard::StructureCard(const StructureCard& other)
    : Card(other),
      effect(other.effect),
      hp(other.hp),
      isDead(other.isDead) {
}

StructureCard::~StructureCard() = default;

void swap(StructureCard& first, StructureCard& second) {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.effect,        second.effect);
    swap(first.hp,            second.hp);
    swap(first.isDead,        second.isDead);
}

StructureCard& StructureCard::operator=(StructureCard other) {
    swap(*this, other);
    return *this;
}

void StructureCard::onPlayEffect() {
    std::cout << "Structure " << name << " has been built on the board\n";
}

void StructureCard::onDeath() {
    if (isDead) return;
    isDead = true;
    hp = 0;
    std::cout << "Structure " << name << " has been destroyed\n";
}

std::unique_ptr<Card> StructureCard::clone() const {
    return std::make_unique<StructureCard>(*this);
}

void StructureCard::displayDetails(std::ostream& os) const {
    os << " [Structure] | HP: " << hp;
    Card::displayDetails(os);
}

void StructureCard::onTurnChange(Game& game) {
    if (!isDead && effect) {
        effect(*this, game);
    }
}

void StructureCard::updateHp(int amount) {
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
