#include "TrapCard.h"
#include "UnitCard.h"
#include <iostream>

TrapCard::TrapCard(const std::string& name, unsigned manaCost,
                   const std::string& effectSummary)
    : TrapCard(name, manaCost, nullptr, effectSummary) {
}

TrapCard::TrapCard(const std::string& name, unsigned manaCost,
                   TrapEffect effect, const std::string& effectSummary)
    : Card(name, manaCost, effectSummary),
      effect(effect),
      isTriggered(false) {
}

TrapCard::TrapCard(const TrapCard& other)
    : Card(other),
      effect(other.effect),
      isTriggered(other.isTriggered) {
}

void swap(TrapCard& first, TrapCard& second) {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.effect,        second.effect);
    swap(first.isTriggered,   second.isTriggered);
}

TrapCard& TrapCard::operator=(TrapCard other) {
    swap(*this, other);
    return *this;
}

void TrapCard::onPlayEffect() {
    std::cout << "Trap " << name << " has been set on the board! [hidden]\n";
}

void TrapCard::onDeath() {
}

std::unique_ptr<Card> TrapCard::clone() const {
    return std::make_unique<TrapCard>(*this);
}

void TrapCard::displayDetails(std::ostream& os) const {
    os << " [Trap]";
    if (isTriggered) {
        os << " [triggered]";
    } else {
        os << " [armed]";
    }
    Card::displayDetails(os);
}

// Declanșează capcana: execută efectul și o marchează ca folosită.
// După declanșare, Game::cleanupBoard() o va elimina de pe tablă.
void TrapCard::trigger(UnitCard& attacker, UnitCard& target) {
    if (isTriggered) return; // Protecție: nu se declanșează de două ori.
    isTriggered = true;
    std::cout << ">>> TRAP ACTIVATED: " << name << "! <<<\n";
    if (effect) {
        effect(attacker, target);
    }
}
