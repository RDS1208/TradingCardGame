# Trading Card Game in C++

A two-player, turn-based console card game written in C++20. The project demonstrates inheritance, polymorphism, modern memory ownership, custom exceptions, and several design patterns.

---

## Project Structure

```text
TradingCardGame/
├── CMakeLists.txt
├── .github/workflows/
│   └── build.yml              # Build and test automation
├── data/
│   └── deck.txt               # Runtime card definitions
├── examples/
│   └── gameplay.txt           # Recorded console session
├── include/
│   ├── Card.h                 # Abstract card interface
│   ├── UnitCard.h             # Unit cards with attack and health
│   ├── SpellCard.h            # One-shot spell cards
│   ├── StructureCard.h        # Structures with turn effects
│   ├── TrapCard.h             # Traps triggered by attacks
│   ├── Player.h               # Deck, hand, and board state
│   ├── Game.h                 # Game rules and turn handling
│   ├── GameHelpers.h          # Console input and display helpers
│   ├── EffectRegistry.h       # Effect lookup
│   ├── CardFactory.h          # Card creation from deck records
│   └── Exceptions.h           # Game exception types
├── src/
│   ├── main.cpp               # Application entry point
│   └── ...                    # Card, game, and effect implementations
└── tests/
    ├── TradingCardGameTests.cpp
    ├── EdgeCaseTests.cpp
    └── TestSupport.h          # Isolated test fixtures
```

CMake copies `data/deck.txt` beside the executable. The game also loads `data/deck.txt` when launched from the project root.

### Build and Run

Requirements: CMake 3.20 or newer and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
./build/TradingCardGame
ctest --test-dir build --output-on-failure
```

For generators such as Visual Studio, build with `--config Release` and test with `-C Release`.

The GitHub Actions workflow builds and tests on Linux, Windows, and macOS. A separate Linux job checks for undefined behavior. Tests use fixed shuffle seeds and unique temporary directories; normal games use random seeds.

To run undefined behavior checks locally with GCC or Clang:

```sh
cmake -S . -B build-sanitized -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-sanitize-recover=all -Wall -Wextra -Wpedantic" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=undefined"
cmake --build build-sanitized
ctest --test-dir build-sanitized --output-on-failure
```

### Understanding the Tests

Each test is a normal C++ function: create the objects, perform an action, and check the result. The `check(condition, message)` helper reports a failure when the condition is false.

For example, a unit with 5 health fighting a unit with 3 attack should have 2 health afterward:

```cpp
UnitCard attacker("Attacker", 1, 5, 5);
UnitCard target("Target", 1, 3, 6);
attacker.onAttack(target);
check(attacker.getHp() == 2, "The attacker should have 2 health");
```

`TradingCardGameTests.cpp` checks card creation and game rules. `EdgeCaseTests.cpp` checks invalid input, health limits, and interrupted combat. Both files call their test functions directly from `main()`.

`TestSupport.h` contains one helper class. It creates a temporary deck file for a test, then restores the working directory and removes that file when the test ends. The tests never replace the project's actual deck.

## How to Play

Each player receives a shuffled copy of the deck and draws four cards at the start. Players take turns. On each turn, a player draws one card and gains one mana, up to a maximum of ten.

The menu lets each player:

1. Play a unit, structure, or trap, or cast a spell from their hand.
2. View their hand.
3. View both boards.
4. Attack an opposing unit.
5. End their turn.
6. Exit the game.

Cards cost between zero and ten mana. A unit can attack once per turn. When units fight, both deal damage at the same time. The defending player's first armed trap also triggers before combat and is removed afterward. Spells are consumed when cast. A player loses after six of their units are defeated. Destroyed structures and triggered traps do not count toward this limit.

Before-attack effects resolve before combat damage. If either unit dies during that effect, combat stops and the attack is spent. Healing saturates at the largest value supported by `int`. Closing input exits the game cleanly.

A short recorded console session is available in [examples/gameplay.txt](examples/gameplay.txt). Card order varies because decks are shuffled.

## Card Types

Every card derives from the abstract `Card` class and implements its type-specific behavior.

| Type | Description | Behavior |
| --- | --- | --- |
| `UnitCard` | A unit with attack and health | Attacks once per turn and can apply optional attack effects. |
| `SpellCard` | A one-shot spell | Resolves its effect and is removed from the hand. |
| `StructureCard` | A structure with health | Remains on the board and can apply an effect when its owner's turn begins. |
| `TrapCard` | A hidden trap | Triggers on an opposing attack, applies its effect, then leaves the board. |

## C++ Concepts Used

### Inheritance and Polymorphism

- The hierarchy is `Card` to `UnitCard`, `SpellCard`, `StructureCard`, and `TrapCard`.
- Pure virtual functions define card behavior, cloning, and display details.
- The Non-Virtual Interface pattern keeps `play()` and `display()` consistent while calling type-specific virtual functions.
- `clone()` provides polymorphic copying and returns `std::unique_ptr<Card>`.

### Memory Management

- `std::unique_ptr<Card>` owns cards in decks, hands, and boards.
- Moving a card transfers its ownership between containers.
- Card classes provide copy operations where polymorphic copies are needed; derived assignments use copy-and-swap.
- `Player` is movable and non-copyable because it owns cards exclusively.

### Exceptions

The custom exception hierarchy derives from `std::exception` and provides errors for deck loading, invalid card data, malformed records, and insufficient mana.

### Other C++ Features

- `dynamic_cast` checks card types when applying type-specific game rules.
- Friend declarations let `Game` and `EffectRegistry` access the closely related player and game state they need.
- Static members track card IDs and unit lifecycle counts.
- Standard library features include `std::vector`, `std::shuffle`, `std::function`, `std::istringstream`, and `std::mt19937`.
- Constructors and member functions use const references and const-correct accessors where appropriate.
- Forward declarations reduce header dependencies.

### Effect System

Card effects are stored as `std::function` callbacks and selected by keys in `data/deck.txt`:

- `UnitCard` supports callbacks before and after an attack.
- `SpellCard` applies an effect to a selected card.
- `StructureCard` applies an effect when the owner's turn begins.
- `TrapCard` applies an effect when an opposing unit attacks.

Effects are defined in `EffectRegistry.cpp`. Existing keys include `FireballEffect`, `HealEffect`, `VampiricStrike`, `HealBase`, and `DamageAttacker`.

### Design Patterns

- **Singleton:** `EffectRegistry::getInstance()` provides the shared effect registry.
- **Simple Factory:** `CardFactory::createCard()` validates a deck record and creates the matching card type.
- **Strategy:** Lambda callbacks provide interchangeable card effects without creating a subclass for every effect.
- **Non-Virtual Interface:** Public base-class operations apply shared behavior before dispatching to virtual card-specific behavior.

### Templates

`GameException<T>` stores typed details alongside an error message. `GameExceptionBase` provides a common catch type, and `printExceptionDetails()` formats the stored detail for the console.

## Deck File Format

Each non-empty line in `data/deck.txt` defines one card. Lines whose first non-space character is `#` are comments. The summary is the remainder of the line, so it may contain additional `|` characters.

```text
Unit|Name|Mana|Attack|HP|EffectKey|EffectSummary
Spell|Name|Mana|EffectKey|EffectSummary
Structure|Name|Mana|HP|EffectKey|EffectSummary
Trap|Name|Mana|EffectKey|EffectSummary
```

Example:

```text
Unit|Knight|4|4|5|None|Heavy charge
Spell|Fireball|2|FireballEffect|Deal 3 damage to a unit.
Structure|Healing Shrine|2|5|HealBase|Heal your units by 1 at the start of your turn.
Trap|Spike Trap|1|DamageAttacker|Deal 2 damage to the attacker.
```

Card records require valid integer fields. Mana must be between zero and ten, and unit attack, unit health, and structure health must be positive. Unsupported card types and effect keys are reported as deck errors with the file and line number.

Empty decks and decks containing only comments or whitespace are rejected.

## Similar Games

- Hearthstone
- Magic: The Gathering
- Yu-Gi-Oh!

## Project Summary

This project implements a two-player trading card game in C++20. It uses a polymorphic card collection with exclusive ownership, loads card definitions from a text file, and handles turns, spells, combat, and board cleanup.

### 1. Object-Oriented Design and Polymorphism

`Card` defines the shared interface for all card types. Its pure virtual functions prevent direct construction and require each derived type to provide its own play, death, cloning, and display behavior. A virtual destructor ensures derived objects are destroyed correctly through base-class pointers.

The Non-Virtual Interface approach keeps `play()` and `display()` as stable public operations while delegating card-specific behavior to virtual functions.

### 2. Smart Pointers and Move Semantics

Decks, hands, and boards store cards in `std::unique_ptr`. Playing or drawing a card transfers ownership with `std::move`, avoiding manual `new` and `delete`. Temporary non-owning `Card*` pointers are used only while accessing a card owned by a container.

### 3. Game Rules and Runtime Type Checks

`Game` validates indices and card types before acting. `dynamic_cast` provides checked access to unit, structure, and trap behavior. Board cleanup removes defeated units, destroyed structures, and triggered traps while counting only defeated units toward the loss condition.

Unit combat applies each unit's damage simultaneously. Trap effects resolve before that combat, and spell effects resolve against the selected board card.

### 4. Generic Exception Handling

`GameException<T>` stores error details with the message. `GameExceptionBase` supports common exception handling, while derived exceptions represent deck errors, invalid card data, and insufficient mana.

### 5. Design Patterns

- **Singleton:** `EffectRegistry` has one shared instance and cannot be copied or moved.
- **Simple Factory:** `CardFactory` parses deck records and creates the corresponding card through a `std::unique_ptr<Card>`.
- **Strategy:** `std::function` callbacks and lambdas inject behavior into cards at construction time.
