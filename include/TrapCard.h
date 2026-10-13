#pragma once
#ifndef TRADINGCARDGAME_TRAPCARD_H
#define TRADINGCARDGAME_TRAPCARD_H

#include "Card.h"
#include <functional>
#include <string>

class UnitCard;

// A hidden board card triggered by an opposing attack.
class TrapCard : public Card {
public:
    // Trap callbacks receive the attacker and its target.
    using TrapEffect = std::function<void(UnitCard& attacker, UnitCard& target)>;

private:
    TrapEffect  effect;
    bool        isTriggered;

public:
    TrapCard(const std::string& name, unsigned manaCost,
             const std::string& effectSummary = "");
    TrapCard(const std::string& name, unsigned manaCost,
             TrapEffect effect, const std::string& effectSummary = "");
    TrapCard(const TrapCard& other);
    ~TrapCard() override = default;
    TrapCard& operator=(TrapCard other);
    friend void swap(TrapCard& first, TrapCard& second);

    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;

    // Runs the effect once and marks the trap as triggered.
    void trigger(UnitCard& attacker, UnitCard& target);

    bool hasTriggered() const { return isTriggered; }
};

#endif // TRADINGCARDGAME_TRAPCARD_H
