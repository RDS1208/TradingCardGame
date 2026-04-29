#include "Player.h"

#include "Card.h"

#include <cstddef>
#include <ostream>
#include <utility>

namespace {

// Funcție ajutătoare: afișează o listă de cărți (mână sau tablă) cu indexul lor.
// Este în 'namespace anonim' — vizibilă doar în acest fișier .cpp.
void displayCardList(const std::vector<std::unique_ptr<Card>>& cards,
                     const char* title,
                     std::ostream& os) {
    os << title << ":\n";
    if (cards.empty()) {
        os << "  (empty)\n";
        return;
    }
    for (std::size_t i = 0; i < cards.size(); ++i) {
        os << "  [" << i << "] ";
        if (cards[i]) {
            // Apel polimorfic: display() apelează displayDetails() specific tipului real.
            cards[i]->display(os);
        } else {
            os << "(null)\n";
        }
    }
}

} // namespace

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
