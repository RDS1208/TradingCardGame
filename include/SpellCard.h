#pragma once
#ifndef TRADINGCARDGAME_SPELLCARD_H
#define TRADINGCARDGAME_SPELLCARD_H

#include "Card.h"
#include <functional>

class SpellCard : public Card {
public:
    // SpellEffect este un alias pentru un tip de funcție care primește un Card*
    // (ținta) și execută un efect (daune, vindecare etc.).
    using SpellEffect = std::function<void(Card*)>;

private:
    SpellEffect  effect;
    std::string  effectDescription;
    Card*        currentTarget; // Pointer brut: vrăjile nu dețin ținta, doar o referențiază temporar.

public:
    SpellCard(const std::string& name, unsigned manaCost,
              const std::string& description = "");
    SpellCard(const std::string& name, unsigned manaCost,
              SpellEffect effect, const std::string& description = "");
    SpellCard(const SpellCard& other);
    ~SpellCard() override = default;
    SpellCard& operator=(SpellCard other);
    friend void swap(SpellCard& first, SpellCard& second);

    // Apelat de Game chiar înainte de play() pentru a seta ținta aleasă de jucător.
    void setTarget(Card* target) { currentTarget = target; }

    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;
};

#endif // TRADINGCARDGAME_SPELLCARD_H
