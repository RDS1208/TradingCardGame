#include "StructureCard.h"
#include "Game.h"
#include <iostream>

// Constructor simplu: delegă la constructorul complet cu un efect gol (nullptr).
StructureCard::StructureCard(const std::string& name, unsigned manaCost, int hp,
                             const std::string& effectSummary)
    : StructureCard(name, manaCost, hp, nullptr, effectSummary) {
}

// Constructor complet: inițializează clasa de bază Card, apoi membrii proprii.
StructureCard::StructureCard(const std::string& name, unsigned manaCost, int hp,
                             TurnChangeEffect effect, const std::string& effectSummary)
    : Card(name, manaCost),
      effect(effect),
      effectSummary(effectSummary),
      hp(hp),
      isDead(false) {
}

// Constructor de copiere: copiază toți membrii din 'other'.
// Card(other) apelează constructorul de copiere al clasei de bază.
StructureCard::StructureCard(const StructureCard& other)
    : Card(other),
      effect(other.effect),
      effectSummary(other.effectSummary),
      hp(other.hp),
      isDead(other.isDead) {
}

// Destructorul nu are nevoie de logică specială (spre deosebire de UnitCard).
StructureCard::~StructureCard() = default;

// Swap: interschimbă toți membrii între două StructureCard-uri.
// static_cast<Card&> face swap și pe partea moștenită din Card.
void swap(StructureCard& first, StructureCard& second) noexcept {
    using std::swap;
    swap(static_cast<Card&>(first), static_cast<Card&>(second));
    swap(first.effect,        second.effect);
    swap(first.effectSummary, second.effectSummary);
    swap(first.hp,            second.hp);
    swap(first.isDead,        second.isDead);
}

// Copy-and-swap: 'other' vine copiat (prin valoare), facem swap, apoi copia e distrusă.
StructureCard& StructureCard::operator=(StructureCard other) {
    swap(*this, other);
    return *this;
}

void StructureCard::onPlayEffect() {
    std::cout << "Structure " << name << " has been built on the board\n";
}

void StructureCard::onDeath() {
    if (isDead) return; // Protecție: nu executăm moartea de două ori.
    isDead = true;
    hp = 0;
    std::cout << "Structure " << name << " has been destroyed\n";
}

// Virtual constructor: creează o copie identică a acestei structuri.
std::unique_ptr<Card> StructureCard::clone() const {
    return std::make_unique<StructureCard>(*this);
}

// Afișează detaliile specifice StructureCard (HP și descrierea efectului).
void StructureCard::displayDetails(std::ostream& os) const {
    os << " [Structure] | HP: " << hp;
    if (!effectSummary.empty()) {
        os << " | Effect: " << effectSummary;
    }
}

// Apelată la fiecare schimbare de tur. Execută efectul doar dacă structura e în viață.
void StructureCard::onTurnChange(Game& game) {
    if (!isDead && effect) {
        effect(*this, game);
    }
}

// Modifică HP-ul structurii. Dacă HP scade sub 0, declanșează moartea.
void StructureCard::updateHp(int amount) {
    if (isDead || amount == 0) return;
    hp += amount;
    if (hp <= 0) {
        onDeath();
    }
}
