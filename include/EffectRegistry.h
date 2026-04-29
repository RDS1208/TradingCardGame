#pragma once
#ifndef TRADINGCARDGAME_EFFECTREGISTRY_H
#define TRADINGCARDGAME_EFFECTREGISTRY_H

#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "UnitCard.h"
#include <string>

// EffectRegistry: fabrică de efecte.
// Traduce un șir de text (citit din deck.txt) într-un lambda C++ real.
// Exemplu: în deck.txt scrie "FireballEffect" → registrul returnează un lambda
// care scade 3 HP de la țintă.
// Această clasă este 'friend' cu Game și Player, așa că lambda-urile pot
// accesa membrii privați (ex: tabla jucătorului) pentru efecte complexe.
class EffectRegistry {
public:
    // Returnează un lambda pentru efectul unei vrăji (SpellCard)
    static SpellCard::SpellEffect  getSpellEffect(const std::string& effectName);
    // Returnează callback-urile de atac pentru un UnitCard
    static UnitCard::AttackEffects getUnitAttackEffects(const std::string& effectName);
    // Returnează un lambda pentru efectul de tur al unui StructureCard
    static StructureCard::TurnChangeEffect getStructureEffect(const std::string& effectName);
    // Returnează un lambda pentru efectul unei capcane (TrapCard)
    static TrapCard::TrapEffect getTrapEffect(const std::string& effectName);
};

#endif // TRADINGCARDGAME_EFFECTREGISTRY_H
