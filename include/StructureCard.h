#pragma once
#ifndef TRADINGCARDGAME_STRUCTURECARD_H
#define TRADINGCARDGAME_STRUCTURECARD_H

#include "Card.h"
#include <functional>
#include <string>

// Declarație anticipată: îi spunem compilatorului că clasa Game există,
// fără să includem tot fișierul Game.h (evităm dependențe circulare).
class Game;

// StructureCard: o clădire/structură plasată pe tablă.
// Nu atacă, dar are HP și un efect care se activează la fiecare schimbare de tur.
class StructureCard : public Card {
public:
    // Alias: o funcție care primește structura și jocul curent.
    // Efectul are nevoie de Game& ca să poată accesa tabla jucătorului (via friend).
    using TurnChangeEffect = std::function<void(StructureCard&, Game&)>;

private:
    TurnChangeEffect effect;        // Efectul care se declanșează la fiecare tur
    std::string      effectSummary; // Descrierea efectului (afișată în consolă)
    int              hp;            // Viața structurii
    bool             isDead;        // true dacă structura a fost distrusă

public:
    // Constructor simplu (fără efect) — delegă la constructorul complet
    StructureCard(const std::string& name, unsigned manaCost, int hp,
                  const std::string& effectSummary = "");
    // Constructor complet (cu efect de tur)
    StructureCard(const std::string& name, unsigned manaCost, int hp,
                  TurnChangeEffect effect, const std::string& effectSummary = "");
    // Constructor de copiere
    StructureCard(const StructureCard& other);
    ~StructureCard() override;
    // Operator de atribuire (copy-and-swap)
    StructureCard& operator=(StructureCard other);
    friend void swap(StructureCard& first, StructureCard& second) noexcept;

    // --- Funcții virtuale moștenite din Card ---
    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;

    // Apelată de Game::changeTurn() la fiecare început de tur
    void onTurnChange(Game& game);

    int  getHp()   const { return hp; }
    bool isAlive() const { return !isDead; }
    void updateHp(int amount);
};

#endif // TRADINGCARDGAME_STRUCTURECARD_H
