#pragma once
#ifndef TRADINGCARDGAME_TRAPCARD_H
#define TRADINGCARDGAME_TRAPCARD_H

#include "Card.h"
#include <functional>
#include <string>

class UnitCard; // Declarație anticipată: avem nevoie de UnitCard doar ca referință.

// TrapCard: o capcană plasată pe tablă.
// Stă ascunsă până când adversarul atacă. Când atacul are loc,
// capcana se declanșează automat, își aplică efectul, apoi dispare.
class TrapCard : public Card {
public:
    // Efectul capcanei primește atacatorul și ținta atacului.
    // Exemplu: poate face daune atacatorului sau poate proteja ținta.
    using TrapEffect = std::function<void(UnitCard& attacker, UnitCard& target)>;

private:
    TrapEffect  effect;        // Lambda-ul care se execută la declanșare
    std::string effectSummary; // Descrierea efectului (afișată în consolă)
    bool        isTriggered;   // true după ce capcana s-a declanșat

public:
    // Constructor simplu (fără efect) — delegă la constructorul complet
    TrapCard(const std::string& name, unsigned manaCost,
             const std::string& effectSummary = "");
    // Constructor complet (cu efect)
    TrapCard(const std::string& name, unsigned manaCost,
             TrapEffect effect, const std::string& effectSummary = "");
    // Constructor de copiere
    TrapCard(const TrapCard& other);
    ~TrapCard() override = default;
    // Operator de atribuire (copy-and-swap)
    TrapCard& operator=(TrapCard other);
    friend void swap(TrapCard& first, TrapCard& second);

    // --- Funcții virtuale moștenite din Card ---
    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;

    // Declanșează capcana: execută efectul și o marchează ca folosită.
    // Apelată automat de Game::tryAttack() când adversarul atacă.
    void trigger(UnitCard& attacker, UnitCard& target);

    bool hasTriggered() const { return isTriggered; }
};

#endif // TRADINGCARDGAME_TRAPCARD_H
