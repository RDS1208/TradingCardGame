# Trading Card Game — Joc de Cărți în C++

Un joc de cărți pe runde pentru doi jucători, implementat în C++ cu accent pe **moștenire**, **polimorfism** și **gestiunea memoriei** prin smart pointers.

---

## Cum se joacă

Fiecare jucător primește un pachet de cărți citit din `data/deck.txt`. La început, fiecare trage 4 cărți în mână. Pe rând, jucătorii pot:

1. **Juca o carte** din mână pe tablă (costă mana)
2. **Vedea mâna** curentă
3. **Vedea ambele table** de joc
4. **Ataca** o unitate adversă cu o unitate proprie
5. **Termina turul**
6. **Ieși din joc**

La fiecare tur nou, jucătorul trage o carte și primește +1 mana (maxim 10).

**Condiția de victorie:** Primul jucător care pierde 6 unități de pe tablă pierde partida.

---

## Tipuri de Cărți

Toate cărțile moștenesc din clasa abstractă `Card` și suprascriu funcțiile virtuale pure.

| Tip | Descriere | Particularitate |
|-----|-----------|-----------------|
| **UnitCard** | Unitate cu atac și HP | Poate ataca o dată pe tur; are efecte opționale de atac (ex: Vampiric Strike) |
| **SpellCard** | Vrajă cu efect instant | Se consumă imediat la joc — nu rămâne pe tablă |
| **StructureCard** | Clădire cu HP și efect pasiv | Nu atacă, dar declanșează un efect la fiecare schimbare de tur |

---

## Arhitectura Proiectului

```
TradingCardGame/
├── include/              ← Header-ele (.h) cu declarațiile claselor
│   ├── Card.h            ← Clasa de bază abstractă
│   ├── UnitCard.h        ← Derivata 1: unitate de luptă
│   ├── SpellCard.h       ← Derivata 2: vrajă instant
│   ├── StructureCard.h   ← Derivata 3: clădire pasivă
│   ├── Exceptions.h      ← Ierarhia de excepții (independentă de Card)
│   ├── Player.h          ← Jucătorul: deck, mână, tablă
│   ├── Game.h            ← Motorul jocului: tururi, reguli, cleanup
│   ├── GameHelpers.h     ← Funcții inline pentru I/O în terminal
│   └── EffectRegistry.h  ← Fabrică de efecte (traduce stringuri în lambdas)
├── src/                  ← Implementările (.cpp)
│   ├── Card.cpp
│   ├── UnitCard.cpp
│   ├── SpellCard.cpp
│   ├── StructureCard.cpp
│   ├── EffectRegistry.cpp
│   ├── Player.cpp
│   ├── Game.cpp
│   └── main.cpp          ← Punctul de intrare al programului
├── data/
│   └── deck.txt          ← Definițiile cărților (citite la rulare)
├── CMakeLists.txt        ← Configurația de build
└── TradingCardGame.exe   ← Executabilul compilat
```

---

## Concepte C++ Utilizate

### Moștenire și Polimorfism
- **Ierarhie de clase:** `Card` → `UnitCard`, `SpellCard`, `StructureCard`
- **Funcții pur virtuale:** `onPlayEffect()`, `onDeath()`, `clone()`
- **Pattern NVI (Non-Virtual Interface):** `play()` apelează `onPlayEffect()` polimorfic
- **Virtual constructors:** `clone()` returnează `std::unique_ptr<Card>`

### Gestiunea Memoriei
- **Smart pointers:** `std::unique_ptr<Card>` peste tot (deck, mână, tablă)
- **Rule of Three:** constructori de copiere, operatori de atribuire (copy-and-swap) și destructori personalizați pentru Card, UnitCard, SpellCard, StructureCard

### Excepții
Ierarhie independentă de ierarhia Card:
- `GameException` ← baza (moștenește din `std::exception`)
  - `DeckLoadException` — fișierul deck.txt nu poate fi deschis
  - `InvalidCardStatException` — carte cu statistici invalide (mana > 10)
  - `NotEnoughManaException` — jucătorul nu are suficientă mana

### Alte Concepte
- **`dynamic_cast`** pentru downcast-uri sigure la procesarea tablei
- **`friend class`** pentru a permite `EffectRegistry` accesul la membrii privați ai `Game` și `Player`
- **`static`** membri și funcții: contoare (`totalCreated`, `totalDestroyed`), constante (`maxMana`, `defeatLimit`)
- **STL:** `std::vector`, `std::shuffle`, `std::swap`, `std::function`
- **`const`-correctness:** parametri, funcții membre, referințe

### Sistemul de Efecte
Cărțile au efecte configurabile prin lambda-uri, stocate ca `std::function`:
- **UnitCard:** `AttackEffects` (onBefore / onAfter atac)
- **SpellCard:** `SpellEffect` (efect instant pe o țintă)
- **StructureCard:** `TurnChangeEffect` (efect la fiecare tur)

Efectele sunt definite în `EffectRegistry.cpp` și mapate prin chei-text din `deck.txt` (ex: `"FireballEffect"`, `"VampiricStrike"`, `"HealBase"`).

---

## Cum se compilează

```bash
cmake -B build
cmake --build build --config Release
```

Executabilul va fi generat ca `TradingCardGame.exe` în folderul principal.
