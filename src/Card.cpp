#include "Card.h"
#include "Exceptions.h"

#include <iostream>
#include <utility>

int Card::nextId = 0;

Card::Card(std::string name, unsigned manaCost)
    // std::move transferă proprietatea șirului în loc să-l copieze — mai eficient.
    : id(nextId++), name(std::move(name)), manaCost(manaCost) {
    if (this->manaCost > 10) {
        throw InvalidCardStatException(this->name);
    }
}

Card::Card(const Card& other)
    // O carte copiată primește un ID nou și unic, nu un duplicat al celui original.
    : id(nextId++), name(other.name), manaCost(other.manaCost) {
}

void swap(Card& first, Card& second) {
    using std::swap;
    swap(first.id,       second.id);
    swap(first.name,     second.name);
    swap(first.manaCost, second.manaCost);
}

Card& Card::operator=(const Card& other) {
    // Protecție împotriva auto-atribuirii: "card = card" nu trebuie să facă nimic.
    if (this != &other) {
        id       = other.id;
        name     = other.name;
        manaCost = other.manaCost;
    }
    return *this;
}

void Card::play() {
    std::cout << "[ID:" << id << "] Card played to field.\n";
    // Apel polimorfic: C++ determină la rulare tipul real și apelează
    // implementarea corectă (UnitCard::onPlayEffect sau SpellCard::onPlayEffect).
    onPlayEffect();
}

void Card::display(std::ostream& os) const {
    os << "[ID:" << id << "] " << name << " | Mana Cost: " << manaCost;
    displayDetails(os);
    os << '\n';
}

void Card::displayDetails(std::ostream&) const {
    // Versiunea de bază nu face nimic intenționat.
    // Clasele derivate suprascriu aceasta pentru a afișa propriile informații.
}
