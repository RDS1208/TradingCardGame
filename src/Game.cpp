#include "Game.h"

#include "CardFactory.h"
#include "UnitCard.h"
#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "EffectRegistry.h"
#include "Exceptions.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

static bool shouldRemoveCardFromBoard(const std::unique_ptr<Card>& card) {
    if (const auto* unit = dynamic_cast<const UnitCard*>(card.get())) {
        return !unit->isAlive();
    }
    if (const auto* structure = dynamic_cast<const StructureCard*>(card.get())) {
        return !structure->isAlive();
    }
    if (const auto* trap = dynamic_cast<const TrapCard*>(card.get())) {
        return trap->hasTriggered();
    }
    return false;
}

static bool isValidIndex(int index, std::size_t size) {
    return index >= 0 && static_cast<std::size_t>(index) < size;
}

static std::vector<std::unique_ptr<Card>> makeBaseDeck() {
    std::vector<std::unique_ptr<Card>> deck;
    std::string filename = "data/deck.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        file.clear();
        filename = "deck.txt";
        file.open(filename);
    }
    if (!file.is_open()) {
        throw DeckLoadException("data/deck.txt or deck.txt");
    }

    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        const auto firstContent = std::find_if_not(line.begin(), line.end(), [](unsigned char character) {
            return std::isspace(character);
        });
        if (firstContent == line.end() || *firstContent == '#') {
            continue;
        }

        try {
            deck.push_back(CardFactory::createCard(line));
        } catch (const GameExceptionBase& error) {
            throw DeckLoadException(filename, lineNumber, error.what());
        }
    }

    if (deck.empty()) {
        throw DeckLoadException(filename, "deck contains no cards");
    }
    return deck;
}


Game::Game(const std::string& playerOneName, const std::string& playerTwoName,
           unsigned int randomSeed)
    : playerOne(playerOneName), playerTwo(playerTwoName), randomEngine(randomSeed) {
    resetPlayers();
}

Player& Game::currentPlayer() {
    return playerOne.isMyTurn ? playerOne : playerTwo;
}

const Player& Game::currentPlayer() const {
    return playerOne.isMyTurn ? playerOne : playerTwo;
}

Player& Game::opposingPlayer() {
    return playerOne.isMyTurn ? playerTwo : playerOne;
}

const Player& Game::opposingPlayer() const {
    return playerOne.isMyTurn ? playerTwo : playerOne;
}

void Game::resetPlayers() {
    playerOne.deck = makeBaseDeck();
    playerTwo.deck = makeBaseDeck();

    playerOne.hand.clear();
    playerTwo.hand.clear();
    playerOne.board.clear();
    playerTwo.board.clear();

    playerOne.defeatedUnitCards = 0;
    playerTwo.defeatedUnitCards = 0;
    playerOne.mana              = 1;
    playerTwo.mana              = 1;
    playerOne.isMyTurn          = true;
    playerTwo.isMyTurn          = false;

    gameOver    = false;
}

void Game::shuffleDeck(Player& player) {
    std::shuffle(player.deck.begin(), player.deck.end(), randomEngine);
}

void Game::drawCards(Player& player, int count) {
    while (count > 0 && !player.deck.empty()) {
        player.hand.push_back(std::move(player.deck.back()));
        player.deck.pop_back();
        --count;
    }
}

void Game::dealStartingHands() {
    shuffleDeck(playerOne);
    shuffleDeck(playerTwo);
    drawCards(playerOne, startingHandSize);
    drawCards(playerTwo, startingHandSize);
}

void Game::cleanupBoard(Player& player) {
    const auto defeatedUnits = std::count_if(
        player.board.begin(), player.board.end(), [](const auto& card) {
            const auto* unit = dynamic_cast<const UnitCard*>(card.get());
            return unit && !unit->isAlive();
        });

    std::erase_if(player.board, shouldRemoveCardFromBoard);
    player.defeatedUnitCards += static_cast<unsigned int>(defeatedUnits);
    if (player.defeatedUnitCards > defeatLimit) {
        player.defeatedUnitCards = defeatLimit;
    }

}

void Game::cleanupDefeatedCards() {
    cleanupBoard(playerOne);
    cleanupBoard(playerTwo);
}

void Game::updateGameOverState() {
    gameOver = playerOne.defeatedUnitCards >= defeatLimit
            || playerTwo.defeatedUnitCards >= defeatLimit;
}

void Game::startGame() {
    resetPlayers();
    dealStartingHands();
}

void Game::changeTurn() {
    if (gameOver) return;

    playerOne.isMyTurn = !playerOne.isMyTurn;
    playerTwo.isMyTurn = !playerTwo.isMyTurn;

    Player& player = currentPlayer();
    drawCards(player, 1);
    if (player.mana < maxMana) {
        ++player.mana;
    }

    for (auto& card : player.board) {
        if (auto* unit = dynamic_cast<UnitCard*>(card.get())) {
            unit->resetAttackStatus();
        }
        else if (auto* structure = dynamic_cast<StructureCard*>(card.get())) {
            structure->onTurnChange(*this);
        }
    }

    cleanupDefeatedCards();
    updateGameOverState();
}

bool Game::isSpellCard(int index) const {
    return dynamic_cast<const SpellCard*>(handCard(index)) != nullptr;
}

bool Game::tryPlayCardFromHand(int index) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (!isValidIndex(index, player.hand.size())) return false;

    std::unique_ptr<Card>& card = player.hand[index];
    if (!card) return false;
    if (dynamic_cast<SpellCard*>(card.get())) return false;

    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName(), card->getManaCost());
    }

    player.mana -= card->getManaCost();
    card->play();

    player.board.push_back(std::move(player.hand[index]));
    player.hand.erase(player.hand.begin() + index);
    return true;
}

bool Game::tryPlaySpellFromHand(int handIndex, int targetIndex, bool isEnemy) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (!isValidIndex(handIndex, player.hand.size())) return false;

    std::unique_ptr<Card>& card = player.hand[handIndex];
    if (!card) return false;

    auto* spell = dynamic_cast<SpellCard*>(card.get());
    if (!spell) return false;

    Card* target = nullptr;
    if (targetIndex >= 0) {
        Player& targetPlayer = isEnemy ? opposingPlayer() : currentPlayer();
        if (!isValidIndex(targetIndex, targetPlayer.board.size())) return false;
        target = targetPlayer.board[targetIndex].get();
        if (!target) return false;
    } else if (targetIndex != -1) {
        return false;
    }

    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName(), card->getManaCost());
    }

    spell->setTarget(target);
    player.mana -= card->getManaCost();
    spell->play();

    player.hand.erase(player.hand.begin() + handIndex);

    cleanupDefeatedCards();
    updateGameOverState();
    return true;
}

bool Game::tryAttack(int attackerIndex, int targetIndex) {
    if (gameOver) return false;

    Player& attackerPlayer = currentPlayer();
    Player& defenderPlayer = opposingPlayer();

    if (!isValidIndex(attackerIndex, attackerPlayer.board.size())
        || !isValidIndex(targetIndex, defenderPlayer.board.size())) {
        return false;
    }

    auto* attacker = dynamic_cast<UnitCard*>(attackerPlayer.board[attackerIndex].get());
    auto* target   = dynamic_cast<UnitCard*>(defenderPlayer.board[targetIndex].get());

    if (!attacker || !target || !attacker->isAlive() || !target->isAlive()) return false;
    if (attacker->hasAttacked()) return false;

    for (auto& card : defenderPlayer.board) {
        auto* trap = dynamic_cast<TrapCard*>(card.get());
        if (trap && !trap->hasTriggered()) {
            trap->trigger(*attacker, *target);
            break;
        }
    }

    if (!attacker->isAlive()) {
        cleanupDefeatedCards();
        updateGameOverState();
        return true;
    }

    attacker->onAttack(*target);

    cleanupDefeatedCards();
    updateGameOverState();
    return true;
}

bool Game::isGameOver() const {
    return gameOver;
}

const std::string& Game::currentPlayerName() const {
    return currentPlayer().getName();
}

unsigned int Game::currentPlayerMana() const {
    return currentPlayer().mana;
}

const Card* Game::handCard(int index) const {
    const auto& hand = currentPlayer().hand;
    return isValidIndex(index, hand.size()) ? hand[index].get() : nullptr;
}

const Card* Game::boardCard(int index, bool isEnemy) const {
    const auto& board = (isEnemy ? opposingPlayer() : currentPlayer()).board;
    return isValidIndex(index, board.size()) ? board[index].get() : nullptr;
}

std::size_t Game::handSize() const {
    return currentPlayer().hand.size();
}

std::size_t Game::boardSize(bool isEnemy) const {
    return (isEnemy ? opposingPlayer() : currentPlayer()).board.size();
}

unsigned int Game::defeatedUnits(bool isEnemy) const {
    return (isEnemy ? opposingPlayer() : currentPlayer()).defeatedUnitCards;
}

void Game::displayCurrentHand(std::ostream& os) const {
    const Player& player = currentPlayer();
    os << player.getName() << "\n";
    player.displayHand(os);
}

void Game::displayBoards(std::ostream& os) const {
    const Player& player = currentPlayer();
    const Player& enemy  = opposingPlayer();
    os << player.getName() << " board:\n";
    player.displayBoard(os);
    os << enemy.getName() << " board:\n";
    enemy.displayBoard(os);
}
