#include "EffectRegistry.h"
#include "Game.h"
#include <iostream>


// Efecte pentru SpellCard (vrăji)
// Fiecare efect primește un Card* (pointer brut la ținta vrăjii).
// Folosim dynamic_cast pentru a verifica dacă ținta e UnitCard.

SpellCard::SpellEffect EffectRegistry::getSpellEffect(const std::string& effectName) {

    if (effectName == "FireballEffect") {
        // Lambda: primeste un Card* si incearca sa-l converteasca la UnitCard*.
        // dynamic_cast returneaza nullptr daca tinta nu este un UnitCard.
        return [](Card* target) {
            if (auto* unit = dynamic_cast<UnitCard*>(target)) {
                std::cout << "Fireball hits " << unit->getName() << " for 3 damage!\n";
                unit->updateHp(-3);
            } else {
                std::cout << "Fireball fizzled; no valid target.\n";
            }
        };
    }

    if (effectName == "HealEffect") {
        return [](Card* target) {
            if (auto* unit = dynamic_cast<UnitCard*>(target)) {
                std::cout << "Heal restores 3 HP to " << unit->getName() << "!\n";
                unit->updateHp(3);
            } else {
                std::cout << "Heal fizzled; no valid target.\n";
            }
        };
    }

    // Nume necunoscut: returnam nullptr (vraja nu face nimic cand e jucata).
    return nullptr;
}


// Efecte de atac pentru UnitCard
// Callback-urile se execută înainte/după aplicarea daunei în onAttack().

UnitCard::AttackEffects EffectRegistry::getUnitAttackEffects(const std::string& effectName) {

    if (effectName == "VampiricStrike") {
        return {
            nullptr, // onBefore: niciun efect înainte de atac

            // onAfter: vindecă atacatorul cu 1 HP după ce dauna a fost aplicată.
            // /*target*/ marchează parametrul ca nefolosit, evitând un warning de compilare.
            [](UnitCard& attacker, UnitCard& /*target*/) {
                attacker.updateHp(1);
                std::cout << attacker.getName() << " healed 1 HP from Vampiric Strike!\n";
            }
        };
    }

    // Nume necunoscut: returnam un struct gol (fara callback-uri).
    return UnitCard::AttackEffects{};
}


// Efecte de tur pentru StructureCard
// Se execută la fiecare schimbare de tur (Game::changeTurn).
// Primește Game& datorită relației 'friend class EffectRegistry' din Game.h,
// ceea ce permite accesul la currentPlayer() și la tabla jucătorului.
StructureCard::TurnChangeEffect EffectRegistry::getStructureEffect(const std::string& effectName) {
    if (effectName == "HealBase") {
        // Acest lambda primeste structura si jocul curent.
        // Datorita 'friend class EffectRegistry' din Player.h si Game.h,
        // putem accesa player.board (privat!) direct.
        return [](StructureCard& structure, Game& game) {
            std::cout << structure.getName() << " heals every unit on your board for 1 HP!\n";
            // game.currentPlayer() e privat, dar EffectRegistry e friend.
            Player& player = game.currentPlayer();
            // Iteram prin tabla jucatorului si vindecam fiecare UnitCard in viata.
            for (auto& card : player.board) {
                // card.get() extrage pointerul brut din unique_ptr pentru dynamic_cast.
                if (auto* unit = dynamic_cast<UnitCard*>(card.get())) {
                    if (unit->isAlive()) {
                        unit->updateHp(1);
                    }
                }
            }
        };
    }
    return nullptr;
}


// Efecte pentru TrapCard (capcane)
// Se declanșează automat când adversarul atacă.
// Primesc atacatorul și ținta ca referințe, permițând modificarea lor directă.
TrapCard::TrapEffect EffectRegistry::getTrapEffect(const std::string& effectName) {
    if (effectName == "DamageAttacker") {
        // Capcana face 2 daune atacatorului înainte ca atacul să se rezolve.
        return [](UnitCard& attacker, UnitCard& /*target*/) {
            std::cout << attacker.getName() << " takes 2 damage from the trap!\n";
            attacker.updateHp(-2);
        };
    }
    return nullptr;
}
