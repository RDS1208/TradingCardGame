#pragma once
#ifndef TRADINGCARDGAME_PLAYER_H
#define TRADINGCARDGAME_PLAYER_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// Avoids including Card.h in this header.
class Card;

class Player {
    friend class Game;
    friend class EffectRegistry;

private:
    std::string                            name;
    std::vector<std::unique_ptr<Card>>     deck;
    std::vector<std::unique_ptr<Card>>     hand;
    std::vector<std::unique_ptr<Card>>     board;
    unsigned int                           defeatedUnitCards = 0;
    unsigned int                           mana              = 1;
    bool                                   isMyTurn          = false;

public:
    Player();
    explicit Player(std::string name);
    
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;
    ~Player();

    const std::string& getName() const;
    void displayCards(std::ostream& os) const;
    void displayHand(std::ostream& os)  const;
    void displayBoard(std::ostream& os) const;
};

#endif // TRADINGCARDGAME_PLAYER_H
