#pragma once
#ifndef TRADINGCARDGAME_CARD_H
#define TRADINGCARDGAME_CARD_H

#include <iosfwd>
#include <memory>
#include <string>

class Card {
protected:
    // ID unic per carte, calculat cu ajutorul contorului static.
    static int  nextId;

    int         id;
    std::string name;
    unsigned    manaCost;
    std::string effectSummary;

public:
    Card(std::string name, unsigned manaCost, std::string effectSummary = "");
    Card(const Card& other);
    Card& operator=(const Card& other);

    // Friend function pentru utilitarul swap (Rule of 5).
    friend void swap(Card& first, Card& second);

    // Destructor virtual: asigura stergerea corecta si completa din memorie 
    // a claselor copil (ex: UnitCard) atunci cand le stergem printr-un pointer de tip Card*.
    virtual ~Card() = default;

    int                getId()       const { return id; }
    const std::string& getName()     const { return name; }
    unsigned           getManaCost() const { return manaCost; }

    void display(std::ostream& os) const;

    // Modelul NVI (Non-Virtual Interface): play() este o functie normala (non-virtuala)
    // care apeleaza in interior functia virtuala onPlayEffect(). 
    // Ne asigura ca exista mereu un comportament comun inainte de efectul specific.
    void play();

    // Functii virtuale pure (= 0): fac clasa sa fie "abstracta" (adica doar un model,
    // nu putem crea un obiect simplu de tip "Card", ci doar clase derivate din el).
    virtual void onPlayEffect() = 0;
    virtual void onDeath()      = 0;
    virtual std::unique_ptr<Card> clone() const = 0;

    // Functie virtuala normala: are o implementare de baza, dar clasele derivate
    // o pot modifica daca au nevoie.
    virtual void displayDetails(std::ostream& os) const;
};

#endif // TRADINGCARDGAME_CARD_H
