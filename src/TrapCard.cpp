#include "TrapCard.h"
#include "UnitCard.h"
#include <iostream>

// Constructor simplu: delegă la constructorul complet cu un efect gol (nullptr).
TrapCard::TrapCard(const std::string& name, unsigned manaCost,
                   const std::string& effectSummary)
    : TrapCard(name, manaCost, nullptr, effectSummary) {
}

// Constructor complet: inițializează clasa de bază Card, apoi membrii proprii.
TrapCard::TrapCard(const std::string& name, unsigned manaCost,
                   TrapEffect effect, const std::string& effectSummary)
    : Card(name, manaCost),
      effect(effect),
      effectSummary(effectSummary),
      isTriggered(false) {
}

// Constructor de copiere: copiază toți membrii din 'other'.
// Card(other) apelează constructorul de copiere al clasei de bază.
TrapCard::TrapCard(const TrapCard& other)
    : Card(other),
      effect(other.effect),
      effectSummary(other.effectSummary),
      isTriggered(other.isTriggered) {
}

// Swap: interschimbă toți membrii între două TrapCard-uri.
// static_cast<Card&> face swap și pe partea moștenită din Card.
void swap(TrapCard& first, TrapCard& second) {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.effect,        second.effect);
    swap(first.effectSummary, second.effectSummary);
    swap(first.isTriggered,   second.isTriggered);
}

// Copy-and-swap: 'other' vine copiat (prin valoare), facem swap, apoi copia e distrusă.
TrapCard& TrapCard::operator=(TrapCard other) {
    swap(*this, other);
    return *this;
}

void TrapCard::onPlayEffect() {
    std::cout << "Trap " << name << " has been set on the board! [hidden]\n";
}

void TrapCard::onDeath() {
    // Capcanele nu au o stare de "moarte" — sunt pur și simplu eliminate după declanșare.
}

// Virtual constructor: creează o copie identică a acestei capcane.
std::unique_ptr<Card> TrapCard::clone() const {
    return std::make_unique<TrapCard>(*this);
}

// Afișează detaliile specifice TrapCard.
// Dacă nu a fost declanșată, arată [armed]. Dacă da, arată [triggered].
void TrapCard::displayDetails(std::ostream& os) const {
    os << " [Trap]";
    if (isTriggered) {
        os << " [triggered]";
    } else {
        os << " [armed]";
    }
    if (!effectSummary.empty()) {
        os << " | Effect: " << effectSummary;
    }
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
