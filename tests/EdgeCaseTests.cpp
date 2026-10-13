#include "Exceptions.h"
#include "GameHelpers.h"
#include "StructureCard.h"
#include "TestSupport.h"
#include "UnitCard.h"

#include <iostream>
#include <limits>
#include <sstream>

namespace {

void check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void testInputEnds() {
    // An empty stream represents closed input.
    std::istringstream input("");
    std::ostringstream output;
    bool inputEnded = false;

    try {
        readInt("Number: ", input, output);
    } catch (const InputEndedException&) {
        inputEnded = true;
    }

    check(inputEnded, "Closed input should stop the reader");
}

void testInvalidInputIsRetried() {
    std::istringstream input("wrong\n42\n");
    std::ostringstream output;

    int number = readInt("Number: ", input, output);

    check(number == 42, "The reader should skip invalid input and read 42");
}

void testUnitHealingCannotOverflow() {
    int maximumHealth = std::numeric_limits<int>::max();
    UnitCard unit("Unit", 0, 1, maximumHealth - 1);

    unit.updateHp(3);

    check(unit.isAlive(), "Healing should not kill a unit");
    check(unit.getHp() == maximumHealth, "Health should stop at the integer limit");
}

void testStructureHealingCannotOverflow() {
    int maximumHealth = std::numeric_limits<int>::max();
    StructureCard structure("Structure", 0, maximumHealth - 1);

    structure.updateHp(3);

    check(structure.isAlive(), "Healing should not destroy a structure");
    check(structure.getHp() == maximumHealth, "Health should stop at the integer limit");
}

void testLargeDamage() {
    UnitCard unit("Unit", 0, 1, 1);
    StructureCard structure("Structure", 0, 1);

    unit.updateHp(std::numeric_limits<int>::min());
    structure.updateHp(std::numeric_limits<int>::min());

    check(!unit.isAlive(), "Large damage should kill the unit");
    check(unit.getHp() == 0, "A defeated unit should have zero health");
    check(!structure.isAlive(), "Large damage should destroy the structure");
    check(structure.getHp() == 0, "A destroyed structure should have zero health");
}

// These functions act as before-attack effects.
void destroyAttacker(UnitCard& attacker, UnitCard&) {
    attacker.onDeath();
}

void destroyTarget(UnitCard&, UnitCard& target) {
    target.onDeath();
}

void testAttackerDiesBeforeCombat() {
    UnitCard::AttackEffects effects;
    effects.onBefore = destroyAttacker;
    UnitCard attacker("Attacker", 0, 5, 5, effects);
    UnitCard target("Target", 0, 1, 10);

    attacker.onAttack(target);

    check(target.getHp() == 10, "A dead attacker should not deal damage");
}

void testTargetDiesBeforeCombat() {
    UnitCard::AttackEffects effects;
    effects.onBefore = destroyTarget;
    UnitCard attacker("Attacker", 0, 5, 5, effects);
    UnitCard target("Target", 0, 1, 10);

    attacker.onAttack(target);

    check(attacker.getHp() == 5, "A dead target should not deal counter-damage");
    check(attacker.hasAttacked(), "The before-attack effect should spend the attack");
}

void testEmptyDeckIsRejected() {
    TestDeckDirectory deck({});
    bool rejected = false;

    try {
        Game game("First", "Second", 42);
    } catch (const DeckLoadException&) {
        rejected = true;
    }

    check(rejected, "An empty deck should be rejected");
}

void testCommentOnlyDeckIsRejected() {
    TestDeckDirectory deck({"  # Comment", "", "  "});
    bool rejected = false;

    try {
        Game game("First", "Second", 42);
    } catch (const DeckLoadException&) {
        rejected = true;
    }

    check(rejected, "A deck containing only comments and spaces should be rejected");
}

} // namespace

int main() {
    try {
        testInputEnds();
        testInvalidInputIsRetried();
        testUnitHealingCannotOverflow();
        testStructureHealingCannotOverflow();
        testLargeDamage();
        testAttackerDiesBeforeCombat();
        testTargetDiesBeforeCombat();
        testEmptyDeckIsRejected();
        testCommentOnlyDeckIsRejected();
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }

    std::cout << "All edge case tests passed.\n";
    return 0;
}
