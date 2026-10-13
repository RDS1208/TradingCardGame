#pragma once
#ifndef TRADINGCARDGAME_GAME_H
#define TRADINGCARDGAME_GAME_H

#include "Player.h"

#include <cstddef>
#include <iosfwd>
#include <random>
#include <string>

// Owns both players and applies the game rules.
class Game {
    // Turn effects need access to the active player.
    friend class EffectRegistry;

private:
    Player       playerOne;
    Player       playerTwo;
    bool         gameOver    = false;
    std::mt19937 randomEngine;

    static constexpr unsigned int maxMana = 10;
    static constexpr int startingHandSize = 4;
    static constexpr unsigned int defeatLimit = 6;

    Player&       currentPlayer();
    const Player& currentPlayer() const;
    Player&       opposingPlayer();
    const Player& opposingPlayer() const;

    void          resetPlayers();
    void          shuffleDeck(Player& player);
    static void   drawCards(Player& player, int count);
    void          dealStartingHands();
    static void   cleanupBoard(Player& player);
    void          cleanupDefeatedCards();
    void          updateGameOverState();

public:
    Game(const std::string& playerOneName = "Player 1",
         const std::string& playerTwoName = "Player 2",
         unsigned int randomSeed = std::random_device{}());

    void startGame();
    void changeTurn();
    bool isSpellCard(int index) const;
    bool tryPlayCardFromHand(int index);
    bool tryPlaySpellFromHand(int handIndex, int targetIndex, bool isEnemy);
    bool tryAttack(int attackerIndex, int targetIndex);
    bool isGameOver() const;

    const std::string& currentPlayerName() const;
    unsigned int       currentPlayerMana() const;
    // Non-owning views remain valid until the next game action.
    const Card* handCard(int index) const;
    const Card* boardCard(int index, bool isEnemy = false) const;
    std::size_t handSize() const;
    std::size_t boardSize(bool isEnemy = false) const;
    unsigned int defeatedUnits(bool isEnemy = false) const;
    void               displayCurrentHand(std::ostream& os) const;
    void               displayBoards(std::ostream& os)      const;
};

#endif // TRADINGCARDGAME_GAME_H
