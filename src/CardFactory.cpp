#include "CardFactory.h"

#include "EffectRegistry.h"
#include "Exceptions.h"
#include "SpellCard.h"
#include "StructureCard.h"
#include "TrapCard.h"
#include "UnitCard.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <system_error>
#include <utility>
#include <vector>

namespace {

std::string trim(std::string value) {
    const auto isNotSpace = [](unsigned char character) {
        return !std::isspace(character);
    };
    const auto first = std::find_if(value.begin(), value.end(), isNotSpace);
    const auto last = std::find_if(value.rbegin(), value.rend(), isNotSpace).base();

    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

std::vector<std::string> readFields(const std::string& record, std::size_t fieldCount) {
    std::vector<std::string> fields;
    fields.reserve(fieldCount);

    std::size_t start = 0;
    for (std::size_t field = 0; field + 1 < fieldCount; ++field) {
        const std::size_t separator = record.find('|', start);
        if (separator == std::string::npos) {
            throw InvalidDeckRecordException("missing field " + std::to_string(field + 2));
        }
        fields.push_back(trim(record.substr(start, separator - start)));
        start = separator + 1;
    }

    fields.push_back(record.substr(start));
    return fields;
}

template <typename Number>
Number parseNumber(const std::string& text, const std::string& fieldName) {
    Number value{};
    const char* first = text.data();
    const char* last = first + text.size();
    const auto [end, error] = std::from_chars(first, last, value);

    if (text.empty() || error != std::errc{} || end != last) {
        throw InvalidDeckRecordException("field '" + fieldName + "' must be a valid integer");
    }
    return value;
}

void validateEffectKey(const std::string& effectKey, bool known, const std::string& cardType) {
    if (effectKey != "None" && !known) {
        throw InvalidDeckRecordException("unknown " + cardType + " effect '" + effectKey + "'");
    }
}

void validateCardName(const std::string& name) {
    if (name.empty()) {
        throw InvalidDeckRecordException("card name cannot be empty");
    }
}

} // namespace

std::unique_ptr<Card> CardFactory::createCard(const std::string& line) {
    const std::size_t typeSeparator = line.find('|');
    const std::string type = trim(line.substr(0, typeSeparator));
    auto& effects = EffectRegistry::getInstance();

    if (type == "Unit") {
        const auto fields = readFields(line, 7);
        validateCardName(fields[1]);
        const unsigned mana = parseNumber<unsigned>(fields[2], "mana");
        const int attack = parseNumber<int>(fields[3], "attack");
        const int hp = parseNumber<int>(fields[4], "HP");
        const std::string& effectKey = fields[5];

        UnitCard::AttackEffects attackEffects;
        if (effectKey != "None") {
            attackEffects = effects.getUnitAttackEffects(effectKey);
            validateEffectKey(effectKey,
                static_cast<bool>(attackEffects.onBefore) || static_cast<bool>(attackEffects.onAfter),
                "unit");
        }
        return std::make_unique<UnitCard>(fields[1], mana, attack, hp, attackEffects, fields[6]);
    }

    if (type == "Spell") {
        const auto fields = readFields(line, 5);
        validateCardName(fields[1]);
        const std::string& effectKey = fields[3];
        SpellCard::SpellEffect effect;
        if (effectKey != "None") {
            effect = effects.getSpellEffect(effectKey);
            validateEffectKey(effectKey, static_cast<bool>(effect), "spell");
        }
        return std::make_unique<SpellCard>(fields[1], parseNumber<unsigned>(fields[2], "mana"),
                                           std::move(effect), fields[4]);
    }

    if (type == "Structure") {
        const auto fields = readFields(line, 6);
        validateCardName(fields[1]);
        const std::string& effectKey = fields[4];
        StructureCard::TurnChangeEffect effect;
        if (effectKey != "None") {
            effect = effects.getStructureEffect(effectKey);
            validateEffectKey(effectKey, static_cast<bool>(effect), "structure");
        }
        return std::make_unique<StructureCard>(fields[1], parseNumber<unsigned>(fields[2], "mana"),
                                               parseNumber<int>(fields[3], "HP"),
                                               std::move(effect), fields[5]);
    }

    if (type == "Trap") {
        const auto fields = readFields(line, 5);
        validateCardName(fields[1]);
        const std::string& effectKey = fields[3];
        TrapCard::TrapEffect effect;
        if (effectKey != "None") {
            effect = effects.getTrapEffect(effectKey);
            validateEffectKey(effectKey, static_cast<bool>(effect), "trap");
        }
        return std::make_unique<TrapCard>(fields[1], parseNumber<unsigned>(fields[2], "mana"),
                                          std::move(effect), fields[4]);
    }

    throw InvalidDeckRecordException("unknown card type '" + type + "'");
}
