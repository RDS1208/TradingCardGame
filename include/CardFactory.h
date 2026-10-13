#pragma once
#ifndef TRADINGCARDGAME_CARDFACTORY_H
#define TRADINGCARDGAME_CARDFACTORY_H

#include "Card.h"
#include <memory>
#include <string>

// Creates cards from validated deck records.
class CardFactory {
public:
    static std::unique_ptr<Card> createCard(const std::string& line);
};

#endif // TRADINGCARDGAME_CARDFACTORY_H
