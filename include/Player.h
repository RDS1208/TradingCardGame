#pragma once
#ifndef TRADINGCARDGAME_PLAYER_H
#define TRADINGCARDGAME_PLAYER_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// Declarație anticipată: compilatorul știe că Card există, fără să includă tot Card.h.
class Card;

// Player reprezintă un jucător cu pachet, mână și tablă de joc.
// Membrii sunt privați, dar Game și EffectRegistry au acces prin 'friend'.
class Player {
    // 'friend class' acordă acces complet la membrii privați.
    // Game are nevoie să mute cărți între deck/mână/tablă.
    // EffectRegistry are nevoie să acceseze tabla pentru efectele StructureCard.
    friend class Game;
    friend class EffectRegistry;

private:
    std::string                            name;
    std::vector<std::unique_ptr<Card>>     deck;  // Pachetul de cărți ne-trase
    std::vector<std::unique_ptr<Card>>     hand;  // Cărțile din mâna jucătorului
    std::vector<std::unique_ptr<Card>>     board; // Cărțile plasate pe tablă
    unsigned int                           defeatedUnitCards = 0; // Câte unități au murit
    unsigned int                           mana              = 1; // Mana disponibilă
    bool                                   isMyTurn          = false;

public:
    Player();
    explicit Player(std::string name);
    // Move constructor și move assignment: permit mutarea unui Player
    // (necesar pentru că unique_ptr nu poate fi copiat, doar mutat).
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;
    ~Player();

    const std::string& getName() const;
    void displayCards(std::ostream& os) const;
    void displayHand(std::ostream& os)  const;
    void displayBoard(std::ostream& os) const;
};

#endif // TRADINGCARDGAME_PLAYER_H
