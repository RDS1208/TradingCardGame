#include "SpellCard.h"
#include <iostream>
#include <utility>

SpellCard::SpellCard(const std::string& name, unsigned manaCost, const std::string& description)
    : Card(name, manaCost), effect(nullptr), effectDescription(description), currentTarget(nullptr) {
}

SpellCard::SpellCard(const std::string& name, unsigned manaCost,
                     SpellEffect effect, const std::string& description)
    // std::move transferă lambda-ul în loc să-l copieze.
    : Card(name, manaCost), effect(std::move(effect)), effectDescription(description), currentTarget(nullptr) {
}

SpellCard::SpellCard(const SpellCard& other)
    : Card(other),
      effect(other.effect),
      effectDescription(other.effectDescription),
      currentTarget(other.currentTarget) {
}

void swap(SpellCard& first, SpellCard& second) {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.effect,            second.effect);
    swap(first.effectDescription, second.effectDescription);
    swap(first.currentTarget,     second.currentTarget);
}

SpellCard& SpellCard::operator=(SpellCard other) {
    swap(*this, other);
    return *this;
}

void SpellCard::onPlayEffect() {
    std::cout << getName() << " is cast!\n";
    if (effect) {
        effect(currentTarget); // Apelează lambda-ul stocat cu ținta selectată.
    }
}

void SpellCard::onDeath() {
    // Vrăjile sunt aruncate după ce sunt jucate; nu au o stare de "moarte".
}

std::unique_ptr<Card> SpellCard::clone() const {
    return std::make_unique<SpellCard>(*this);
}

void SpellCard::displayDetails(std::ostream& os) const {
    os << " [Spell]";
    if (!effectDescription.empty()) {
        os << " Effect: " << effectDescription;
    }
}
