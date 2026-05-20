#pragma once
#ifndef TRADINGCARDGAME_CARDFACTORY_H
#define TRADINGCARDGAME_CARDFACTORY_H

#include "Card.h"
#include <memory>
#include <string>

// O fabrica (Simple Factory) care creeaza carti din liniile de text din deck.txt.
// Am scos logica asta din Game.cpp ca sa fie mai curat codul.
class CardFactory {
public:
    // Folosim std::unique_ptr ca sa evitam memory leaks si sa scutim grija lui delete.
    static std::unique_ptr<Card> createCard(const std::string& line);
};

#endif // TRADINGCARDGAME_CARDFACTORY_H
