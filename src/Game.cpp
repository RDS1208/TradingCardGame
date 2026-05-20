#include "Game.h"

#include "CardFactory.h"
#include "UnitCard.h"
#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "EffectRegistry.h"
#include "Exceptions.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

static std::mt19937& randomEngine() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

static bool isDefeatedCard(const std::unique_ptr<Card>& card) {
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

static std::vector<std::unique_ptr<Card>> makeBaseDeck(const std::string& filename = "deck.txt") {
    std::vector<std::unique_ptr<Card>> deck;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw DeckLoadException(filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::unique_ptr<Card> card = CardFactory::createCard(line);
        if (card) {
            deck.push_back(std::move(card));
        }
    }

    return deck;
}


Game::Game(const std::string& playerOneName, const std::string& playerTwoName)
    : playerOne(playerOneName), playerTwo(playerTwoName) {
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

    turnCounter = 1;
    gameOver    = false;
}

void Game::shuffleDeck(Player& player) {
    std::shuffle(player.deck.begin(), player.deck.end(), randomEngine());
}

void Game::drawCards(Player& player, int count) {
    while (count > 0 && !player.deck.empty()) {
        // Mutăm ultima carte din pachet în mână (std::move evită copierea).
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

unsigned int Game::cleanupBoard(Player& player) {
    std::vector<std::unique_ptr<Card>> survivors;
    unsigned int removed = 0;

    // Trecem prin toate cartile de pe tabla.
    // Daca o carte este infranta (distrusa/activata), o numaram ca fiind eliminata.
    // Daca este inca activa, o salvam in lista de supravietuitori.
    for (auto& card : player.board) {
        if (isDefeatedCard(card)) {
            ++removed;
        } else {
            survivors.push_back(std::move(card));
        }
    }

    // Inlocuim tabla veche cu noua lista de carti supravietuitoare.
    player.board = std::move(survivors);

    player.defeatedUnitCards += removed;
    // Plafonam numarul total de carti infrante pentru a nu depasi limita jocului.
    if (player.defeatedUnitCards > defeatLimit) {
        player.defeatedUnitCards = defeatLimit;
    }

    return removed;
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

    ++turnCounter;
    // Comutăm turul: operatorul ! inversează valoarea bool-ului.
    playerOne.isMyTurn = !playerOne.isMyTurn;
    playerTwo.isMyTurn = !playerTwo.isMyTurn;

    Player& player = currentPlayer();
    drawCards(player, 1);
    if (player.mana < maxMana) {
        ++player.mana;
    }

    // Resetăm starea de atac a tuturor unităților de pe tablă pentru noul tur.
    // Și declanșăm efectele de turn pentru StructureCards.
    for (auto& card : player.board) {
        // .get() extrage pointerul brut din unique_ptr pentru dynamic_cast.
        if (auto* unit = dynamic_cast<UnitCard*>(card.get())) {
            unit->resetAttackStatus();
        }
        else if (auto* structure = dynamic_cast<StructureCard*>(card.get())) {
            structure->onTurnChange(*this);
        }
    }
}

bool Game::isSpellCard(int index) const {
    const Player& player = currentPlayer();
    if (index < 0 || index >= (int)player.hand.size()) return false;
    // dynamic_cast returnează nullptr dacă nu este SpellCard → bool false.
    return dynamic_cast<SpellCard*>(player.hand[index].get()) != nullptr;
}

bool Game::tryPlayCardFromHand(int index) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (index < 0 || index >= (int)player.hand.size()) return false;

    Card* card = player.hand[index].get();
    if (!card) return false;

    // Verificăm costul de mană
    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName(), card->getManaCost());
    }

    player.mana -= card->getManaCost();
    card->play();

    // Mutăm cartea din mână direct pe tablă.
    player.board.push_back(std::move(player.hand[index]));
    player.hand.erase(player.hand.begin() + index);
    return true;
}

bool Game::tryPlaySpellFromHand(int handIndex, int targetIndex, bool isEnemy) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (handIndex < 0 || handIndex >= (int)player.hand.size()) return false;

    Card* card = player.hand[handIndex].get();
    if (!card) return false;

    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName(), card->getManaCost());
    }

    auto* spell = dynamic_cast<SpellCard*>(card);
    if (!spell) return false;

    // Identificăm ținta vrăjii, dacă există
    Card* target = nullptr;
    if (targetIndex >= 0) {
        Player& targetPlayer = isEnemy ? opposingPlayer() : currentPlayer();
        if (targetIndex < (int)targetPlayer.board.size()) {
            target = targetPlayer.board[targetIndex].get();
        }
    }

    spell->setTarget(target);
    player.mana -= card->getManaCost();
    spell->play();

    // Vrăjile nu rămân pe tablă, ci sunt șterse din mână după utilizare.
    player.hand.erase(player.hand.begin() + handIndex);

    cleanupDefeatedCards();
    updateGameOverState();
    return true;
}

bool Game::tryAttack(int attackerIndex, int targetIndex) {
    if (gameOver) return false;

    Player& attackerPlayer = currentPlayer();
    Player& defenderPlayer = opposingPlayer();

    if (attackerIndex < 0 || attackerIndex >= (int)attackerPlayer.board.size() || 
        targetIndex < 0 || targetIndex >= (int)defenderPlayer.board.size()) {
        return false;
    }

    auto* attacker = dynamic_cast<UnitCard*>(attackerPlayer.board[attackerIndex].get());
    auto* target   = dynamic_cast<UnitCard*>(defenderPlayer.board[targetIndex].get());

    // Verificăm că ambele cărți există, sunt unități și sunt în viață.
    if (!attacker || !target || !attacker->isAlive() || !target->isAlive()) return false;
    if (attacker->hasAttacked()) return false; // O unitate poate ataca o singură dată pe tur.

    // --- Verificăm capcanele adversarului ---
    // Dacă jucătorul care se apără are TrapCard-uri pe tablă, le declanșăm.
    for (auto& card : defenderPlayer.board) {
        auto* trap = dynamic_cast<TrapCard*>(card.get());
        if (trap && !trap->hasTriggered()) {
            trap->trigger(*attacker, *target);
            break; // Se declanșează doar o singură capcană per atac.
        }
    }

    // Dacă atacatorul a murit din cauza capcanei, atacul nu mai continuă.
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
