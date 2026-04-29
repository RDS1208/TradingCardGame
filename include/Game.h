#pragma once
#ifndef TRADINGCARDGAME_GAME_H
#define TRADINGCARDGAME_GAME_H

#include "Player.h"

#include <cstddef>
#include <iosfwd>
#include <string>

// Game: clasa principală care gestionează întreaga logică a jocului.
// Conține doi jucători, urmărește tururile și aplică regulile.
class Game {
    // EffectRegistry are nevoie de acces la currentPlayer() pentru efectele de tur.
    friend class EffectRegistry;

private:
    Player       playerOne;
    Player       playerTwo;
    unsigned int turnCounter = 1;     // Numărul turului curent
    bool         gameOver    = false; // Devine true când un jucător pierde

    // 'static constexpr' = constante care există la compilare, nu ocupă memorie la rulare.
    static constexpr unsigned int  maxMana          = 10; // Mana maximă pe tur
    static constexpr std::size_t   startingHandSize = 4;  // Câte cărți primește fiecare jucător la început
    static constexpr unsigned int  defeatLimit      = 6;  // Câte unități moarte = pierdere

    // --- Funcții private: logica internă, inaccesibilă din afară ---
    Player&       currentPlayer();         // Returnează jucătorul al cărui tur este
    const Player& currentPlayer() const;
    Player&       opposingPlayer();        // Returnează adversarul
    const Player& opposingPlayer() const;

    void          resetPlayers();           // Resetează tot pentru un joc nou
    static void   shuffleDeck(Player& player);  // Amestecă pachetul
    static void   drawCards(Player& player, std::size_t count); // Trage cărți din pachet
    void          dealStartingHands();     // Distribuie mâinile inițiale
    static unsigned int cleanupBoard(Player& player); // Șterge cărțile moarte de pe tablă
    void          cleanupDefeatedCards();   // Curăță ambele table
    void          updateGameOverState();    // Verifică dacă cineva a pierdut

public:
    Game(const std::string& playerOneName = "Player 1",
         const std::string& playerTwoName = "Player 2");

    // --- Funcții publice: interfata folosită din main.cpp ---
    void startGame();                      // Inițializează și împart cărțile
    void changeTurn();                     // Trece la următorul tur
    bool isSpellCard(std::size_t index) const; // Verifică dacă o carte din mână e vrajă
    bool tryPlayCardFromHand(std::size_t index);  // Joacă o carte (Unit/Structure) pe tablă
    bool tryPlaySpellFromHand(std::size_t handIndex, int targetIndex, bool isEnemy); // Lansează o vrajă
    bool tryAttack(std::size_t attackerIndex, std::size_t targetIndex); // Atacă o țintă
    bool isGameOver() const;

    const std::string& currentPlayerName() const;
    unsigned int       currentPlayerMana() const;
    void               displayCurrentHand(std::ostream& os) const;
    void               displayBoards(std::ostream& os)      const;
};

#endif // TRADINGCARDGAME_GAME_H
