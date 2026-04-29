# Trading Card Game — Joc de Cărți în C++

Un joc de cărți pe runde pentru doi jucători, implementat în C++ (standard C++20) cu accent pe **moștenire**, **polimorfism**, **gestiunea memoriei** prin smart pointers și **sistemul de excepții**.

---

## Structura Proiectului

```
TradingCardGame/
├── CMakeLists.txt             # Configurare build (CMake ≥ 4.2, C++20)
├── data/
│   └── deck.txt               # Pachetul de cărți — citit la rulare
├── include/
│   ├── Card.h                 # Clasa abstractă de bază
│   ├── UnitCard.h             # Unitate cu atac și HP
│   ├── SpellCard.h            # Vrajă cu efect instant
│   ├── StructureCard.h        # Clădire cu efect pasiv pe tur
│   ├── TrapCard.h             # Capcană declanșată la atacul adversarului
│   ├── Player.h               # Jucător: deck, mână, tablă
│   ├── Game.h                 # Logica principală a jocului
│   ├── GameHelpers.h          # Funcții inline de interfață consolă
│   ├── EffectRegistry.h       # Fabrică de efecte (lambda-uri)
│   └── Exceptions.h           # Ierarhia de excepții proprii
└── src/
    ├── main.cpp               # Punctul de intrare
    ├── Card.cpp
    ├── UnitCard.cpp
    ├── SpellCard.cpp
    ├── StructureCard.cpp
    ├── TrapCard.cpp
    ├── Player.cpp
    ├── Game.cpp
    └── EffectRegistry.cpp
```

---

## Cum se compilează și rulează

```bash
cmake -S . -B build
cmake --build build --config Release
./TradingCardGame.exe
```

Pachetul de cărți (`data/deck.txt`) este copiat automat lângă executabil prin regula `POST_BUILD` din `CMakeLists.txt`.

---

## Cum se joacă

Fiecare jucător primește un pachet de cărți citit din `deck.txt`. La început, fiecare trage 4 cărți în mână. Pe rând, jucătorii pot:

1. **Juca o carte** din mână pe tablă (costă mana)
2. **Vedea mâna** curentă
3. **Vedea ambele table** de joc
4. **Ataca** o unitate adversă cu o unitate proprie
5. **Termina turul**
6. **Ieși din joc**

La fiecare tur nou, jucătorul trage o carte și primește +1 mana (maxim 10).

**Condiția de victorie:** Primul jucător care pierde 6 unități de pe tablă pierde meciul.

---

## Tipuri de Cărți

Toate cărțile moștenesc din clasa abstractă `Card` și suprascriu funcțiile virtuale pure.

| Tip | Descriere | Particularitate |
|-----|-----------|-----------------|
| **UnitCard** | Unitate cu atac și HP | Poate ataca o dată pe tur; are efecte opționale de atac (ex: Vampiric Strike) |
| **SpellCard** | Vrajă cu efect instant | Se consumă imediat la joc — nu rămâne pe tablă |
| **StructureCard** | Clădire cu HP și efect pasiv | Nu atacă, dar declanșează un efect la fiecare schimbare de tur |
| **TrapCard** | Capcană ascunsă | Stă pe tablă până când adversarul atacă; se declanșează automat, apoi dispare |

---

## Concepte C++ Utilizate

### Moștenire și Polimorfism
- **Ierarhie de clase:** `Card` → `UnitCard`, `SpellCard`, `StructureCard`, `TrapCard`
- **Funcții pur virtuale:** `onPlayEffect()`, `onDeath()`, `clone()`, `displayDetails()`
- **Pattern NVI (Non-Virtual Interface):** `play()` apelează `onPlayEffect()` polimorfic; `display()` apelează `displayDetails()`
- **Virtual constructors:** `clone()` returnează `std::unique_ptr<Card>`

### Gestiunea Memoriei
- **Smart pointers:** `std::unique_ptr<Card>` peste tot (deck, mână, tablă)
- **Copy-control complet:** constructor de copiere, operator de atribuire (copy-and-swap idiom) și destructor pentru `Card`, `UnitCard`, `SpellCard`, `StructureCard`, `TrapCard`
- **Move semantics:** `Player` este move-only (move constructor / move assignment = default; copy dezactivat implicit de `unique_ptr`)

### Excepții
Ierarhie independentă de ierarhia Card:
- `GameException` ← baza (moștenește din `std::exception`)
  - `DeckLoadException` — fișierul deck.txt nu poate fi deschis
  - `InvalidCardStatException` — carte cu statistici invalide (mana > 10)
  - `NotEnoughManaException` — jucătorul nu are suficientă mana

### Alte Concepte
- **`dynamic_cast`** pentru downcast-uri sigure la procesarea tablei
- **`friend class`** — `EffectRegistry` și `Game` accesează membrii privați ai `Player`; `EffectRegistry` accesează membrii privați ai `Game`
- **`friend` funcții libere** — `swap()` pentru fiecare clasă din ierarhie (copy-and-swap idiom)
- **`static`** membri și funcții: contoare (`totalCreated`, `totalDestroyed`, `nextId`), constante (`maxMana`, `defeatLimit`, `startingHandSize`), funcții helper cu legătură internă
- **STL:** `std::vector`, `std::shuffle`, `std::swap`, `std::function`, `std::istringstream`, `std::mt19937`
- **`const`-correctness:** parametri, funcții membre, referințe
- **Delegating constructors:** constructorii simpli delegă la constructorii compleți (ex: `TrapCard`, `StructureCard`)
- **Forward declarations:** `class Game;` în `StructureCard.h`, `class Card;` în `Player.h`, `class UnitCard;` în `TrapCard.h` — evită dependențe circulare

### Sistemul de Efecte
Cărțile au efecte configurabile prin lambda-uri, stocate ca `std::function`:
- **UnitCard:** `AttackEffects` (onBefore / onAfter atac)
- **SpellCard:** `SpellEffect` (efect instant pe o țintă)
- **StructureCard:** `TurnChangeEffect` (efect la fiecare tur)
- **TrapCard:** `TrapEffect` (efect la declanșare, primește atacatorul și ținta)

Efectele sunt definite în `EffectRegistry.cpp` și mapate prin chei-text din `deck.txt` (ex: `"FireballEffect"`, `"VampiricStrike"`, `"HealBase"`, `"DamageAttacker"`).

---

## Formatul Fișierului deck.txt

Fiecare linie descrie o carte. Liniile goale și cele care încep cu `#` sunt ignorate.

```
Unit|Name|Mana|Attack|HP|EffectKey|EffectSummary
Spell|Name|Mana|EffectKey|EffectSummary
Structure|Name|Mana|HP|EffectKey|EffectSummary
Trap|Name|Mana|EffectKey|EffectSummary
```

Exemplu:
```
Unit|Knight|4|4|5|None|Heavy charge
Spell|Fireball|2|FireballEffect|Deal 3 damage to any unit.
Structure|Healing Shrine|2|5|HealBase|Heals all units 1 HP each turn.
Trap|Spike Trap|1|DamageAttacker|Deals 2 damage to the attacker.
```
