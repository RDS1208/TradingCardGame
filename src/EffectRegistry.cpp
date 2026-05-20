#include "EffectRegistry.h"
#include "Game.h"
#include <iostream>

EffectRegistry& EffectRegistry::getInstance() {
    static EffectRegistry instance;
    return instance;
}

SpellCard::SpellEffect EffectRegistry::getSpellEffect(const std::string& effectName) {
    if (effectName == "FireballEffect") {
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
    return nullptr;
}

UnitCard::AttackEffects EffectRegistry::getUnitAttackEffects(const std::string& effectName) {
    if (effectName == "VampiricStrike") {
        return {
            nullptr,
            [](UnitCard& attacker, UnitCard& /*target*/) {
                attacker.updateHp(1);
                std::cout << attacker.getName() << " healed 1 HP from Vampiric Strike!\n";
            }
        };
    }
    return UnitCard::AttackEffects{};
}

StructureCard::TurnChangeEffect EffectRegistry::getStructureEffect(const std::string& effectName) {
    if (effectName == "HealBase") {
        return [](StructureCard& structure, Game& game) {
            std::cout << structure.getName() << " heals every unit on your board for 1 HP!\n";
            Player& player = game.currentPlayer();
            for (auto& card : player.board) {
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

TrapCard::TrapEffect EffectRegistry::getTrapEffect(const std::string& effectName) {
    if (effectName == "DamageAttacker") {
        return [](UnitCard& attacker, UnitCard& /*target*/) {
            std::cout << attacker.getName() << " takes 2 damage from the trap!\n";
            attacker.updateHp(-2);
        };
    }
    return nullptr;
}
