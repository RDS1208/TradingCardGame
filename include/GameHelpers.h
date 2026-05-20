#pragma once
#ifndef TRADINGCARDGAME_GAMEHELPERS_H
#define TRADINGCARDGAME_GAMEHELPERS_H

#include "Game.h"

#include <iostream>
#include <limits>
#include <string>

inline void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline int readInt(const std::string& prompt) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid number. Try again.\n";
        clearInput();
    }
}

inline void showTurnMenu(const Game& game) {
    std::cout << "----------------------------------------\n";
    std::cout << "Current player: " << game.currentPlayerName() << "\n";
    std::cout << "Remaining mana: " << game.currentPlayerMana() << "\n";
    std::cout << "1) Play card from hand\n";
    std::cout << "2) See hand\n";
    std::cout << "3) See both boards\n";
    std::cout << "4) Attack\n";
    std::cout << "5) End turn\n";
    std::cout << "6) Exit Game\n";
}

inline void handlePlayCard(Game& game) {
    game.displayCurrentHand(std::cout);
    const int index = readInt("Card index from hand (-1 to cancel): ");
    if (index < 0) {
        std::cout << "Play cancelled.\n";
        return;
    }

    if (game.isSpellCard(index)) {
        game.displayBoards(std::cout);
        const int targetIndex = readInt("Target index on board (-1 for no target): ");
        if (targetIndex < 0) {
            const bool played = game.tryPlaySpellFromHand(index, -1, false);
            std::cout << (played ? "Spell cast (no target).\n" : "Cannot cast that spell.\n");
        } else {
            const int isEnemy = readInt("Is target enemy? (1 = yes, 0 = no): ");
            const bool played = game.tryPlaySpellFromHand(index, targetIndex, isEnemy == 1);
            std::cout << (played ? "Spell cast on target.\n" : "Cannot cast that spell.\n");
        }
    } else {
        const bool played = game.tryPlayCardFromHand(index);
        std::cout << (played ? "Card played.\n" : "Cannot play that card.\n");
    }
}

inline void handleAttack(Game& game) {
    game.displayBoards(std::cout);

    const int attackerIndex = readInt("Your attacker index (-1 to cancel): ");
    if (attackerIndex < 0) {
        std::cout << "Attack cancelled.\n";
        return;
    }

    const int targetIndex = readInt("Opponent target index (-1 to cancel): ");
    if (targetIndex < 0) {
        std::cout << "Attack cancelled.\n";
        return;
    }

    const bool attacked = game.tryAttack(attackerIndex, targetIndex);
    std::cout << (attacked ? "Attack resolved.\n" : "Attack is not valid.\n");
}

#endif // TRADINGCARDGAME_GAMEHELPERS_H
