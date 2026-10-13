#pragma once
#ifndef TRADINGCARDGAME_CARD_H
#define TRADINGCARDGAME_CARD_H

#include <iosfwd>
#include <memory>
#include <string>

class Card {
protected:
    // Assigns each card a unique ID.
    static int  nextId;

    int         id;
    std::string name;
    unsigned    manaCost;
    std::string effectSummary;

public:
    Card(std::string name, unsigned manaCost, std::string effectSummary = "");
    Card(const Card& other);
    Card& operator=(const Card& other);

    friend void swap(Card& first, Card& second);

    // Allows safe deletion through a base pointer.
    virtual ~Card() = default;

    int                getId()       const { return id; }
    const std::string& getName()     const { return name; }
    unsigned           getManaCost() const { return manaCost; }

    void display(std::ostream& os) const;

    // Keeps shared play behavior consistent across card types.
    void play();

    virtual void onPlayEffect() = 0;
    virtual void onDeath()      = 0;
    virtual std::unique_ptr<Card> clone() const = 0;

    virtual void displayDetails(std::ostream& os) const;
};

#endif // TRADINGCARDGAME_CARD_H
