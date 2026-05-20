#pragma once
#ifndef TRADINGCARDGAME_EFFECTREGISTRY_H
#define TRADINGCARDGAME_EFFECTREGISTRY_H

#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "UnitCard.h"
#include <string>

// Clasa Singleton pentru gestiunea efectelor cartilor.
// Asociaza un string (citit din deck.txt) cu logica efectiva (lambda-uri).
// Am folosit un std::function pentru efecte ca o implementare de Strategy pattern.
// Clasa Singleton: asigura ca in toata memoria jocului va exista o SINGURA instanta
// (o singura copie) a acestui registru. Constructorul este privat din acest motiv.
class EffectRegistry {
private:
    EffectRegistry() = default;

public:
    // Interzicem copierea sau mutarea registrului (cu '= delete') 
    // pentru a impiedica duplicarea accidentala in memorie.
    EffectRegistry(const EffectRegistry&) = delete;
    EffectRegistry& operator=(const EffectRegistry&) = delete;
    EffectRegistry(EffectRegistry&&) = delete;
    EffectRegistry& operator=(EffectRegistry&&) = delete;

    // Metoda publica statica pentru a obtine acces la singura instanta existenta.
    static EffectRegistry& getInstance();

    // Metode de instanță (nu mai sunt statice)
    SpellCard::SpellEffect  getSpellEffect(const std::string& effectName);
    UnitCard::AttackEffects getUnitAttackEffects(const std::string& effectName);
    StructureCard::TurnChangeEffect getStructureEffect(const std::string& effectName);
    TrapCard::TrapEffect getTrapEffect(const std::string& effectName);
};

#endif // TRADINGCARDGAME_EFFECTREGISTRY_H
