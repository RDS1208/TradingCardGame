#pragma once
#ifndef TRADINGCARDGAME_STRUCTURECARD_H
#define TRADINGCARDGAME_STRUCTURECARD_H

#include "Card.h"
#include <functional>
#include <string>

// Avoids including Game.h here.
class Game;

// A board structure with health and a turn-change effect.
class StructureCard : public Card {
public:
    using TurnChangeEffect = std::function<void(StructureCard&, Game&)>;

private:
    TurnChangeEffect effect;
    int              hp;
    bool             isDead;

public:
    StructureCard(const std::string& name, unsigned manaCost, int hp,
                  const std::string& effectSummary = "");
    StructureCard(const std::string& name, unsigned manaCost, int hp,
                  TurnChangeEffect effect, const std::string& effectSummary = "");
    StructureCard(const StructureCard& other);
    ~StructureCard() override;
    StructureCard& operator=(StructureCard other);
    friend void swap(StructureCard& first, StructureCard& second);

    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;

    void onTurnChange(Game& game);

    int  getHp()   const { return hp; }
    bool isAlive() const { return !isDead; }
    void updateHp(int amount);
};

#endif // TRADINGCARDGAME_STRUCTURECARD_H
