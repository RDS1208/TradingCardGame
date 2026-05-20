#include "Player.h"

#include "Card.h"

#include <cstddef>
#include <ostream>
#include <utility>

// Cuvantul cheie 'static' in fata unei functii dintr-un fisier .cpp
// inseamna ca aceasta functie poate fi apelata DOAR in interiorul acestui fisier.
// Este un mod excelent de a evita conflictele de nume cu alte fisiere.
static void displayCardList(const std::vector<std::unique_ptr<Card>>& cards,
                            const char* title,
                            std::ostream& os) {
    os << title << ":\n";
    if (cards.empty()) {
        os << "  (empty)\n";
        return;
    }
    int size = (int)cards.size();
    for (int i = 0; i < size; ++i) {
        os << "  [" << i << "] ";
        if (cards[i]) {
            cards[i]->display(os);
        } else {
            os << "(null)\n";
        }
    }
}

Player::Player() = default;

Player::Player(std::string name)
    : name(std::move(name)) {
}

Player::~Player() = default;

const std::string& Player::getName() const {
    return name;
}

void Player::displayCards(std::ostream& os) const {
    os << name << "\n";
    displayHand(os);
    displayBoard(os);
}

void Player::displayHand(std::ostream& os) const {
    displayCardList(hand, "Hand", os);
}

void Player::displayBoard(std::ostream& os) const {
    displayCardList(board, "Board", os);
}
