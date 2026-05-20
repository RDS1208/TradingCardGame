#include "CardFactory.h"
#include "UnitCard.h"
#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "EffectRegistry.h"
#include <sstream>
#include <vector>

static std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::unique_ptr<Card> CardFactory::createCard(const std::string& line) {
    std::vector<std::string> parts = splitString(line, '|');
    if (parts.empty()) return nullptr;

    if (parts[0] == "Unit" && parts.size() >= 7) {
        const std::string& name      = parts[1];
        unsigned           mana      = std::stoi(parts[2]);
        int                attack    = std::stoi(parts[3]);
        int                hp        = std::stoi(parts[4]);
        const std::string& effectKey = parts[5];
        const std::string& summary   = parts[6];

        if (effectKey != "None") {
            UnitCard::AttackEffects effects = EffectRegistry::getInstance().getUnitAttackEffects(effectKey);
            return std::make_unique<UnitCard>(name, mana, attack, hp, effects, summary);
        } else {
            return std::make_unique<UnitCard>(name, mana, attack, hp, summary);
        }
    }
    else if (parts[0] == "Spell" && parts.size() >= 5) {
        const std::string&     name      = parts[1];
        unsigned               mana      = std::stoi(parts[2]);
        const std::string&     effectKey = parts[3];
        const std::string&     summary   = parts[4];
        SpellCard::SpellEffect effect    = EffectRegistry::getInstance().getSpellEffect(effectKey);
        return std::make_unique<SpellCard>(name, mana, effect, summary);
    }
    else if (parts[0] == "Structure" && parts.size() >= 6) {
        const std::string& name      = parts[1];
        unsigned           mana      = std::stoi(parts[2]);
        int                hp        = std::stoi(parts[3]);
        const std::string& effectKey = parts[4];
        const std::string& summary   = parts[5];
        
        StructureCard::TurnChangeEffect effect = EffectRegistry::getInstance().getStructureEffect(effectKey);
        return std::make_unique<StructureCard>(name, mana, hp, effect, summary);
    }
    else if (parts[0] == "Trap" && parts.size() >= 5) {
        const std::string& name      = parts[1];
        unsigned           mana      = std::stoi(parts[2]);
        const std::string& effectKey = parts[3];
        const std::string& summary   = parts[4];

        TrapCard::TrapEffect effect = EffectRegistry::getInstance().getTrapEffect(effectKey);
        return std::make_unique<TrapCard>(name, mana, effect, summary);
    }

    return nullptr;
}
