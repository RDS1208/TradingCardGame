#include "CardFactory.h"
#include "Exceptions.h"
#include "Game.h"
#include "SpellCard.h"
#include "StructureCard.h"
#include "TestSupport.h"
#include "TrapCard.h"
#include "UnitCard.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// A false condition stops the test and explains what went wrong.
void check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void testFactoryCreatesUnits() {
    auto card = CardFactory::createCard("Unit|Knight|4|4|5|None|Heavy charge");

    check(dynamic_cast<UnitCard*>(card.get()) != nullptr, "The factory should create a unit");
}

void testFactoryCreatesSpells() {
    auto card = CardFactory::createCard("Spell|Fireball|2|FireballEffect|Deal 3 damage");

    check(dynamic_cast<SpellCard*>(card.get()) != nullptr, "The factory should create a spell");
}

void testFactoryCreatesStructures() {
    auto card = CardFactory::createCard("Structure|Shrine|2|5|HealBase|Heal units");

    check(dynamic_cast<StructureCard*>(card.get()) != nullptr,
        "The factory should create a structure");
}

void testFactoryCreatesTraps() {
    auto card = CardFactory::createCard("Trap|Spike|1|DamageAttacker|Deal 2 damage");

    check(dynamic_cast<TrapCard*>(card.get()) != nullptr, "The factory should create a trap");
}

bool recordIsRejected(const std::string& record) {
    try {
        CardFactory::createCard(record);
    } catch (const GameExceptionBase&) {
        return true;
    }
    return false;
}

void testInvalidRecordsAreRejected() {
    const std::vector<std::string> invalidRecords = {
        "Unit|Knight|4|4",                              // Missing fields.
        "Unit|Knight|wrong|4|5|None|Summary",           // Invalid number.
        "Dragon|Knight|4|4|5|None|Summary",             // Unknown type.
        "Unit||4|4|5|None|Summary",                    // Empty name.
        "Spell|Fireball|2|UnknownEffect|Summary",
        "Unit|Knight|4|4|5|UnknownEffect|Summary",
        "Structure|Shrine|2|5|UnknownEffect|Summary",
        "Trap|Spike|1|UnknownEffect|Summary",
        "Unit|Knight|999999999999999999999|4|5|None|Summary",
        "Unit|Knight|-1|4|5|None|Summary"               // Negative mana.
    };

    for (const std::string& record : invalidRecords) {
        check(recordIsRejected(record), "The factory should reject: " + record);
    }
}

void testInvalidUnitStatsAreRejected() {
    bool attackRejected = false;
    try {
        UnitCard unit("Unit", 1, 0, 2);
    } catch (const InvalidCardStatException&) {
        attackRejected = true;
    }
    check(attackRejected, "A unit should not start with zero attack");

    bool healthRejected = false;
    try {
        UnitCard unit("Unit", 1, 1, 0);
    } catch (const InvalidCardStatException&) {
        healthRejected = true;
    }
    check(healthRejected, "A unit should not start with zero health");
}

void testInvalidStructureHealthIsRejected() {
    bool rejected = false;
    try {
        StructureCard structure("Structure", 1, 0);
    } catch (const InvalidCardStatException&) {
        rejected = true;
    }

    check(rejected, "A structure should not start with zero health");
}

void testSummaryKeepsExtraSeparators() {
    auto card = CardFactory::createCard("Unit|Knight|4|4|5|None|Heavy charge|Extra detail");
    std::ostringstream output;

    card->display(output);

    check(output.str().find("Heavy charge|Extra detail") != std::string::npos,
        "The full summary should appear in the card description");
}

void testDeckErrorsIncludeLineNumbers() {
    TestDeckDirectory deck({"  # Comment", "", "Unit|Knight|4|4|5|None|Summary",
                            "Spell|Bad|2|MissingEffect|Summary"});
    std::string message;

    try {
        Game game("First", "Second", 42);
    } catch (const DeckLoadException& error) {
        message = error.what();
    }

    check(message.find("deck.txt:4") != std::string::npos,
        "The error should identify line 4 of deck.txt");
}

void testInvalidActionsKeepMana() {
    Game game("First", "Second", 42);
    unsigned int manaBefore = game.currentPlayerMana();

    check(!game.tryPlayCardFromHand(-1), "A negative card index should be rejected");
    check(!game.tryPlayCardFromHand(100), "A missing card index should be rejected");
    check(game.currentPlayerMana() == manaBefore, "Invalid actions should keep the player's mana");
}

void testSpellsCannotBePlacedOnTheBoard() {
    TestDeckDirectory deck({"Spell|Fireball|0|FireballEffect|Deal 3 damage"});
    Game game("First", "Second", 42);
    game.startGame();
    unsigned int manaBefore = game.currentPlayerMana();

    bool played = game.tryPlayCardFromHand(0);

    check(!played, "A spell should use the spell action");
    check(game.isSpellCard(0), "The rejected spell should stay in hand");
    check(game.boardSize() == 0, "The spell should not appear on the board");
    check(game.currentPlayerMana() == manaBefore, "The rejected spell should not spend mana");
}

void testInvalidSpellTargetsKeepTheCard() {
    TestDeckDirectory deck({"Spell|Fireball|0|FireballEffect|Deal 3 damage"});
    Game game("First", "Second", 42);
    game.startGame();
    unsigned int manaBefore = game.currentPlayerMana();

    bool played = game.tryPlaySpellFromHand(0, 100, true);

    check(!played, "A missing spell target should be rejected");
    check(game.isSpellCard(0), "The spell should stay in hand");
    check(game.currentPlayerMana() == manaBefore, "The invalid spell should not spend mana");
}

void testDeckLoadsFromDataFolder() {
    TestDeckDirectory deck({"Unit|Unit|0|1|2|None|Summary"}, "data/deck.txt");
    Game game("First", "Second", 42);
    game.startGame();

    check(game.tryPlayCardFromHand(0), "A card from data/deck.txt should be playable");
}

void testSameSeedGivesTheSameHand() {
    std::vector<std::string> records;
    for (int number = 0; number < 10; ++number) {
        records.push_back("Unit|Unit " + std::to_string(number) + "|0|1|2|None|Summary");
    }
    TestDeckDirectory deck(records);
    Game first("First", "Second", 42);
    Game second("First", "Second", 42);
    first.startGame();
    second.startGame();

    check(first.handSize() == 4, "The opening hand should contain four cards");
    check(second.handSize() == 4, "The second game should also deal four cards");
    for (int index = 0; index < 4; ++index) {
        check(first.handCard(index)->getName() == second.handCard(index)->getName(),
            "Matching seeds should give matching cards");
    }
    check(first.handCard(-1) == nullptr, "A negative hand index should return no card");
    check(first.boardCard(100) == nullptr, "A missing board index should return no card");
}

void testBothUnitsTakeCombatDamage() {
    UnitCard attacker("Attacker", 1, 5, 5);
    UnitCard target("Target", 1, 3, 6);

    attacker.onAttack(target);

    check(target.getHp() == 1, "The target should have 6 - 5 = 1 health");
    check(attacker.getHp() == 2, "The attacker should have 5 - 3 = 2 health");
}

void testSixUnitLossesEndTheGame() {
    // Every card is a free unit that dies in one fight.
    TestDeckDirectory deck(std::vector<std::string>(16, "Unit|Unit|0|5|1|None|Summary"));
    Game game("First", "Second", 42);
    game.startGame();

    for (unsigned int fight = 1; fight <= 6; ++fight) {
        check(game.tryPlayCardFromHand(0), "First should play a unit");
        game.changeTurn();
        check(game.tryPlayCardFromHand(0), "Second should play a unit");

        check(game.tryAttack(0, 0), "Both units should fight and die");

        check(game.defeatedUnits() == fight, "Second should lose one unit per fight");
        check(game.defeatedUnits(true) == fight, "First should lose one unit per fight");
        if (fight < 6) {
            check(!game.isGameOver(), "Fewer than six losses should not end the game");
            game.changeTurn();
        }
    }

    check(game.isGameOver(), "The sixth unit loss should end the game");
}

void testTriggeredTrapDoesNotCountAsAUnitLoss() {
    // Both players receive one unit and one trap.
    TestDeckDirectory deck({"Unit|Unit|0|1|10|None|Summary",
                            "Trap|Trap|0|None|Summary"});
    Game game("First", "Second", 42);
    game.startGame();

    int unitIndex = 0;
    if (dynamic_cast<const UnitCard*>(game.handCard(0)) == nullptr) unitIndex = 1;
    check(game.tryPlayCardFromHand(unitIndex), "First should play the unit");
    game.changeTurn();

    unitIndex = 0;
    if (dynamic_cast<const UnitCard*>(game.handCard(0)) == nullptr) unitIndex = 1;
    check(game.tryPlayCardFromHand(unitIndex), "Second should play the unit");
    check(game.tryPlayCardFromHand(0), "Second should play the remaining trap");
    game.changeTurn();

    check(game.boardSize(true) == 2, "Second should have a unit and an armed trap");
    check(game.tryAttack(0, 0), "First should attack and trigger the trap");

    check(game.boardSize(true) == 1, "Only Second's unit should remain");
    check(game.defeatedUnits(true) == 0, "The removed trap should not count as a defeated unit");
    check(game.defeatedUnits() == 0, "First's unit should still be alive");
}

} // namespace

int main() {
    try {
        testFactoryCreatesUnits();
        testFactoryCreatesSpells();
        testFactoryCreatesStructures();
        testFactoryCreatesTraps();
        testInvalidRecordsAreRejected();
        testInvalidUnitStatsAreRejected();
        testInvalidStructureHealthIsRejected();
        testSummaryKeepsExtraSeparators();
        testDeckErrorsIncludeLineNumbers();
        testInvalidActionsKeepMana();
        testSpellsCannotBePlacedOnTheBoard();
        testInvalidSpellTargetsKeepTheCard();
        testDeckLoadsFromDataFolder();
        testSameSeedGivesTheSameHand();
        testBothUnitsTakeCombatDamage();
        testSixUnitLossesEndTheGame();
        testTriggeredTrapDoesNotCountAsAUnitLoss();
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }

    std::cout << "All trading card game tests passed.\n";
    return 0;
}
