#include "Game.h"

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
#include <sstream>
#include <utility>

// Returnează un singur motor de numere aleatorii, inițializat o dată la pornirea programului.
// 'static' local înseamnă că obiectul este creat prima dată când funcția e apelată
// și trăiește până la terminarea programului.
static std::mt19937& randomEngine() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

// Verifică dacă o carte de pe tablă este o carte moartă (UnitCard sau StructureCard).
// dynamic_cast returnează nullptr dacă pointerul nu este de tipul corect.
static bool isDefeatedCard(const std::unique_ptr<Card>& card) {
    if (const auto* unit = dynamic_cast<const UnitCard*>(card.get())) {
        return !unit->isAlive();
    }
    if (const auto* structure = dynamic_cast<const StructureCard*>(card.get())) {
        return !structure->isAlive();
    }
    // O capcană declanșată trebuie eliminată de pe tablă.
    if (const auto* trap = dynamic_cast<const TrapCard*>(card.get())) {
        return trap->hasTriggered();
    }
    return false;
}

// Împarte un șir de text după un delimitator și returnează un vector de token-uri.
// Exemplu: "Unit|Squire|1|1|2" cu delimitator '|' → {"Unit","Squire","1","1","2"}
static std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Citește fișierul deck.txt și construiește un vector de cărți.
// Fișierul este citit la rulare, deci poți modifica cărțile fără să recompilezi.
static std::vector<std::unique_ptr<Card>> makeBaseDeck(const std::string& filename = "deck.txt") {
    std::vector<std::unique_ptr<Card>> deck;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw DeckLoadException(filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue; // Sărim liniile goale și comentariile din fișier.
        }

        std::vector<std::string> parts = splitString(line, '|');
        if (parts.empty()) continue;

        if (parts[0] == "Unit" && parts.size() >= 7) {
            const std::string& name      = parts[1];
            // std::stoul convertește un string în unsigned long; îl convertim la unsigned.
            unsigned           mana      = static_cast<unsigned>(std::stoul(parts[2]));
            int                attack    = std::stoi(parts[3]);
            int                hp        = std::stoi(parts[4]);
            const std::string& effectKey = parts[5];
            const std::string& summary   = parts[6];

            if (effectKey != "None") {
                UnitCard::AttackEffects effects = EffectRegistry::getUnitAttackEffects(effectKey);
                deck.push_back(std::make_unique<UnitCard>(name, mana, attack, hp, effects, summary));
            } else {
                deck.push_back(std::make_unique<UnitCard>(name, mana, attack, hp, summary));
            }
        }
        else if (parts[0] == "Spell" && parts.size() >= 5) {
            const std::string&     name      = parts[1];
            unsigned               mana      = static_cast<unsigned>(std::stoul(parts[2]));
            const std::string&     effectKey = parts[3];
            const std::string&     summary   = parts[4];
            SpellCard::SpellEffect effect    = EffectRegistry::getSpellEffect(effectKey);
            deck.push_back(std::make_unique<SpellCard>(name, mana, effect, summary));
        }
        else if (parts[0] == "Structure" && parts.size() >= 6) {
            const std::string& name      = parts[1];
            unsigned           mana      = static_cast<unsigned>(std::stoul(parts[2]));
            int                hp        = std::stoi(parts[3]);
            const std::string& effectKey = parts[4];
            const std::string& summary   = parts[5];
            
            StructureCard::TurnChangeEffect effect = EffectRegistry::getStructureEffect(effectKey);
            deck.push_back(std::make_unique<StructureCard>(name, mana, hp, effect, summary));
        }
        // Parsare linie Trap: Trap|Name|Mana|EffectKey|EffectSummary
        else if (parts[0] == "Trap" && parts.size() >= 5) {
            const std::string& name      = parts[1];
            unsigned           mana      = static_cast<unsigned>(std::stoul(parts[2]));
            const std::string& effectKey = parts[3];
            const std::string& summary   = parts[4];

            TrapCard::TrapEffect effect = EffectRegistry::getTrapEffect(effectKey);
            deck.push_back(std::make_unique<TrapCard>(name, mana, effect, summary));
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

void Game::drawCards(Player& player, std::size_t count) {
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
    unsigned int removed = 0;
    std::size_t  index   = 0;

    while (index < player.board.size()) {
        if (isDefeatedCard(player.board[index])) {
            // Swap-and-pop: mutăm carta moartă la final, apoi o ștergem.
            // Evită deplasarea întregului vector (mai eficient decât erase la mijloc).
            std::swap(player.board[index], player.board.back());
            player.board.pop_back();
            ++removed;
            continue; // Nu incrementăm index: noua carte la [index] trebuie verificată.
        }
        ++index;
    }

    player.defeatedUnitCards += removed;
    // Plafonăm la defeatLimit pentru a evita depășiri accidentale.
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

bool Game::isSpellCard(std::size_t index) const {
    const Player& player = currentPlayer();
    if (index >= player.hand.size()) return false;
    // dynamic_cast returnează nullptr dacă nu este SpellCard → bool false.
    return dynamic_cast<SpellCard*>(player.hand[index].get()) != nullptr;
}

bool Game::tryPlayCardFromHand(std::size_t index) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (index >= player.hand.size()) return false;

    std::unique_ptr<Card>& card = player.hand[index];
    if (!card) return false;
    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName());
    }

    player.mana -= card->getManaCost();
    card->play();
    // std::move transferă proprietatea din mână pe tablă; pointerul din mână devine null.
    player.board.push_back(std::move(card));
    // Ștergem slotul null rămas în mână după move.
    player.hand.erase(player.hand.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

bool Game::tryPlaySpellFromHand(std::size_t handIndex, int targetIndex, bool isEnemy) {
    if (gameOver) return false;

    Player& player = currentPlayer();
    if (handIndex >= player.hand.size()) return false;

    std::unique_ptr<Card>& card = player.hand[handIndex];
    if (!card) return false;
    if (card->getManaCost() > player.mana) {
        throw NotEnoughManaException(card->getName());
    }

    auto* spell = dynamic_cast<SpellCard*>(card.get());
    if (!spell) return false;

    Card* target = nullptr;
    if (targetIndex >= 0) {
        // Alegem tabla inamicului sau a jucătorului curent în funcție de isEnemy.
        Player& targetPlayer = isEnemy ? opposingPlayer() : currentPlayer();
        if (static_cast<std::size_t>(targetIndex) < targetPlayer.board.size()) {
            target = targetPlayer.board[targetIndex].get();
        }
    }

    spell->setTarget(target);
    player.mana -= card->getManaCost();
    spell->play();
    // Vraja este aruncată după joc — nu merge pe tablă, ci dispare din mână.
    player.hand.erase(player.hand.begin() + static_cast<std::ptrdiff_t>(handIndex));

    cleanupDefeatedCards();
    updateGameOverState();
    return true;
}

bool Game::tryAttack(std::size_t attackerIndex, std::size_t targetIndex) {
    if (gameOver) return false;

    Player& attackerPlayer = currentPlayer();
    Player& defenderPlayer = opposingPlayer();

    if (attackerIndex >= attackerPlayer.board.size() || targetIndex >= defenderPlayer.board.size()) {
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
