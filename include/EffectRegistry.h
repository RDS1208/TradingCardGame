#pragma once
#ifndef TRADINGCARDGAME_EFFECTREGISTRY_H
#define TRADINGCARDGAME_EFFECTREGISTRY_H

#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "UnitCard.h"
#include <string>

// Maps deck effect keys to card behavior callbacks.
class EffectRegistry {
private:
    EffectRegistry() = default;

public:
    EffectRegistry(const EffectRegistry&) = delete;
    EffectRegistry& operator=(const EffectRegistry&) = delete;
    EffectRegistry(EffectRegistry&&) = delete;
    EffectRegistry& operator=(EffectRegistry&&) = delete;

    static EffectRegistry& getInstance();

    SpellCard::SpellEffect  getSpellEffect(const std::string& effectName);
    UnitCard::AttackEffects getUnitAttackEffects(const std::string& effectName);
    StructureCard::TurnChangeEffect getStructureEffect(const std::string& effectName);
    TrapCard::TrapEffect getTrapEffect(const std::string& effectName);
};

#endif // TRADINGCARDGAME_EFFECTREGISTRY_H
