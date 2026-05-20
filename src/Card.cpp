#include "Card.h"
#include "Exceptions.h"

#include <iostream>
#include <utility>

int Card::nextId = 0;

Card::Card(std::string name, unsigned manaCost, std::string effectSummary)
    : id(nextId++), name(std::move(name)), manaCost(manaCost), effectSummary(std::move(effectSummary)) {
    if (this->manaCost > 10) {
        throw InvalidCardStatException(this->name, this->manaCost);
    }
}

Card::Card(const Card& other)
    : id(nextId++), name(other.name), manaCost(other.manaCost), effectSummary(other.effectSummary) {
}

void swap(Card& first, Card& second) {
    using std::swap;
    swap(first.id,       second.id);
    swap(first.name,     second.name);
    swap(first.manaCost, second.manaCost);
    swap(first.effectSummary, second.effectSummary);
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        id       = other.id;
        name     = other.name;
        manaCost = other.manaCost;
        effectSummary = other.effectSummary;
    }
    return *this;
}

void Card::play() {
    std::cout << "[ID:" << id << "] Card played to field.\n";
    onPlayEffect();
}

void Card::display(std::ostream& os) const {
    os << "[ID:" << id << "] " << name << " | Mana Cost: " << manaCost;
    displayDetails(os);
    os << '\n';
}

void Card::displayDetails(std::ostream& os) const {
    if (!effectSummary.empty()) {
        os << " | Effect: " << effectSummary;
    }
}
