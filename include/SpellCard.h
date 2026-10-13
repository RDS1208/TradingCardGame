#pragma once
#ifndef TRADINGCARDGAME_SPELLCARD_H
#define TRADINGCARDGAME_SPELLCARD_H

#include "Card.h"
#include <functional>

class SpellCard : public Card {
public:
    // Spell callbacks receive a non-owning target pointer.
    using SpellEffect = std::function<void(Card*)>;

private:
    SpellEffect  effect;
    Card*        currentTarget;

public:
    SpellCard(const std::string& name, unsigned manaCost,
              const std::string& description = "");
    SpellCard(const std::string& name, unsigned manaCost,
              SpellEffect effect, const std::string& description = "");
    SpellCard(const SpellCard& other);
    ~SpellCard() override = default;
    SpellCard& operator=(SpellCard other);
    friend void swap(SpellCard& first, SpellCard& second);

    void setTarget(Card* target) { currentTarget = target; }

    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;
};

#endif // TRADINGCARDGAME_SPELLCARD_H
