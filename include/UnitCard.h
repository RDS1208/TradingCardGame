#pragma once
#ifndef TRADINGCARDGAME_UNITCARD_H
#define TRADINGCARDGAME_UNITCARD_H

#include "Card.h"
#include <functional>

class UnitCard : public Card {
private:
    // Contoare statice — aparțin clasei, nu unui obiect individual.
    // Sunt actualizate în constructori și destructor pentru a urmări câte cărți există.
    static int totalCreated;
    static int totalDestroyed;

public:
    // AttackAction este un alias pentru un tip de funcție care primește doi UnitCard
    // și nu returnează nimic. Permite stocarea de lambdas ca și cum ar fi date obișnuite.
    using AttackAction = std::function<void(UnitCard&, UnitCard&)>;

    // Grupează cele două callback-uri opționale într-o singură structură pentru claritate.
    struct AttackEffects {
        AttackAction onBefore = nullptr; // Se execută înainte de aplicarea daunei
        AttackAction onAfter  = nullptr; // Se execută după aplicarea daunei
    };

private:
    AttackAction onAttackBefore;
    AttackAction onAttackAfter;
    std::string  attackEffectSummary;
    int          attack;
    int          hp;
    bool         isDead;
    bool         hasAttackedThisTurn;

public:
    UnitCard(const std::string& name, unsigned manaCost, int attack, int hp,
             const std::string& attackEffectSummary = "");
    UnitCard(const std::string& name, unsigned manaCost, int attack, int hp,
             const AttackEffects& effects, const std::string& attackEffectSummary = "");
    UnitCard(const UnitCard& other);
    ~UnitCard() override;
    UnitCard& operator=(UnitCard other);
    friend void swap(UnitCard& first, UnitCard& second);

    void onPlayEffect() override;
    void onDeath() override;
    std::unique_ptr<Card> clone() const override;
    void displayDetails(std::ostream& os) const override;
    void onAttack(UnitCard& targetCard);

    int  getAttack()   const { return attack; }
    int  getHp()       const { return hp; }
    bool isAlive()     const { return !isDead; }
    bool hasAttacked() const { return hasAttackedThisTurn; }
    void resetAttackStatus() { hasAttackedThisTurn = false; }

    void updateHp(int amount);
    void clearAttackEffects();

    static int getTotalCreated()   { return totalCreated; }
    static int getTotalDestroyed() { return totalDestroyed; }
    static int getAliveCount()     { return totalCreated - totalDestroyed; }
};

#endif // TRADINGCARDGAME_UNITCARD_H
