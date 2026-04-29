#pragma once
#ifndef TRADINGCARDGAME_CARD_H
#define TRADINGCARDGAME_CARD_H

#include <iosfwd>
#include <memory>
#include <string>

class Card {
protected:
    // 'static' înseamnă că acest contor este comun întregii clase, nu per-obiect.
    // Fiecare carte nouă preia valoarea curentă, apoi o incrementează.
    static int  nextId;

    int         id;
    std::string name;
    unsigned    manaCost;

public:
    Card(std::string name, unsigned manaCost);
    Card(const Card& other);
    Card& operator=(const Card& other);

    // 'friend' acordă acestei funcții libere acces la membrii privați de mai sus.
    friend void swap(Card& first, Card& second) noexcept;

    // Destructorul 'virtual' este obligatoriu când se folosește moștenirea.
    // Fără el, ștergerea unui obiect derivat prin pointer de bază cauzează scurgeri de memorie.
    virtual ~Card() = default;

    int                getId()       const { return id; }
    const std::string& getName()     const { return name; }
    unsigned           getManaCost() const { return manaCost; }

    void display(std::ostream& os) const;

    // Pattern NVI: play() este punctul de intrare public. Execută logica comună,
    // apoi apelează onPlayEffect() pe care fiecare clasă derivată o definește diferit.
    void play();

    // '= 0' face funcțiile "pur virtuale": fiecare clasă derivată TREBUIE să le suprascrie.
    // Asta face Card abstractă — nu poți crea un obiect de tip Card direct.
    virtual void onPlayEffect() = 0;
    virtual void onDeath()      = 0;
    virtual std::unique_ptr<Card> clone() const = 0;

    // Nu e pur virtuală: clasele derivate POT suprascrie asta, dar nu sunt obligate.
    virtual void displayDetails(std::ostream& os) const;
};

#endif // TRADINGCARDGAME_CARD_H
