#pragma once
#ifndef TRADINGCARDGAME_UNITCARD_H
#define TRADINGCARDGAME_UNITCARD_H

#include "Card.h"
#include <functional>

class UnitCard : public Card {
private:
    // Track unit lifecycle counts.
    static int totalCreated;
    static int totalDestroyed;

public:
    using AttackAction = std::function<void(UnitCard&, UnitCard&)>;

    struct AttackEffects {
        AttackAction onBefore = nullptr;
        AttackAction onAfter  = nullptr;
    };

private:
    AttackAction onAttackBefore;
    AttackAction onAttackAfter;
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
