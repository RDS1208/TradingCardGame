#include "Player.h"

#include "Card.h"

#include <cstddef>
#include <ostream>
#include <utility>

static void displayCardList(const std::vector<std::unique_ptr<Card>>& cards,
                            const char* title,
                            std::ostream& os) {
    os << title << ":\n";
    if (cards.empty()) {
        os << "  (empty)\n";
        return;
    }
    for (std::size_t index = 0; index < cards.size(); ++index) {
        os << "  [" << index << "] ";
        if (cards[index]) {
            cards[index]->display(os);
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
