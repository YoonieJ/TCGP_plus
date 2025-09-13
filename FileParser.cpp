// FileParser.cpp
#include "FileParser.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <string>
#include <omp.h>

// Parses an integer from a string, returning 0 if parsing fails.
// Parameters:
// - raw: The string to parse.
// Returns:
// - The parsed integer, or 0 if parsing fails.
int parseIntOrZero(const std::string &raw) {
    std::string s = trim(raw);
    // Remove any trailing non-digit characters.
    while (!s.empty() && !isdigit(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
    if (s.empty()) return 0;
    try {
        return std::stoi(s);
    } catch (...) {
        return 0;
    }
}

// Parses a single SkillEffect token into a SpecialSkill structure.
// Parameters:
// - token: The string token representing the skill effect.
// - effect: The SpecialSkill structure to populate with the parsed effect.
void applySkillEffectToken(const std::string &token, SpecialSkill &effect) {
    if (token == "None") return;

    if (token.rfind("Heal:", 0) == 0) {
        effect.heal = parseIntOrZero(token.substr(5)); // Parse Heal amount.
    } else if (token.rfind("CoinFlip:", 0) == 0) {
        effect.doCoinFlips = true;
        effect.damagePerFlip = parseIntOrZero(token.substr(9)); // Parse damage per flip.
    } else if (token == "CoinFlip:ParalyzeOpp") {
        effect.doCoinFlips = true;
        effect.numFlips = 1;
        effect.paralyzeOpp = true;
    } else if (token == "ShuffleBackIfHeads") {
        effect.doCoinFlips = true;
        effect.numFlips = 1;
        effect.shuffleOpponentBackIfHeads = true;
    } else if (token.rfind("DamagePerEnergy:", 0) == 0 || token.rfind("energyAttached:", 0) == 0) {
        auto sep = token.find(':');
        effect.damagePerEnergyAttached = parseIntOrZero(token.substr(sep + 1)); // Parse damage per energy.
    } else if (token.rfind("randomDmg:", 0) == 0) {
        auto parts = splitAndTrim(token.substr(10), ',');
        if (parts.size() == 2) {
            effect.randomHitDamage = parseIntOrZero(parts[0]); // Parse random hit damage.
            effect.randomHitCount = parseIntOrZero(parts[1]);  // Parse random hit count.
        }
    } else if (token == "PoisonOpp") {
        effect.poisonOpp = true;
    } else if (token == "switchOut") {
        effect.switchOutOpp = true;
    } else if (token == "banSupporter" || token == "BanSupporter:nextTurn") {
        effect.banSupporter = true;
    } else if (token.rfind("dmgIfPoisoned:", 0) == 0) {
        effect.extraDmgIfPoisoned = parseIntOrZero(token.substr(14)); // Parse extra damage if poisoned.
    } else if (token.rfind("reduceDmg:", 0) == 0) {
        effect.damageReduction = parseIntOrZero(token.substr(10)); // Parse damage reduction.
    } else if (token.rfind("benchedDmg:", 0) == 0) {
        effect.benchedDamage = parseIntOrZero(token.substr(11)); // Parse benched damage.
    } else {
        std::cerr << "Warning: Unrecognized SkillEffect token: " << token << std::endl;
    }
}

// Parses a Pokémon block from the input stream.
// Parameters:
// - in: The input stream containing the Pokémon data.
// - p: The Pokémon object to populate with the parsed data.
// Returns:
// - True if the Pokémon block was successfully parsed, false otherwise.
bool parsePokemonBlock(std::istream &in, Pokemon &p) {
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line == "END_POKEMON") return true;

        auto delimPos = line.find(':');
        if (delimPos == std::string::npos) continue;
        std::string key = trim(line.substr(0, delimPos));
        std::string value = trim(line.substr(delimPos + 1));

        if (key == "Name") {
            p.name = (value == "None" ? "" : value);
        } else if (key == "ex") {
            p.isEx = (toLower(value) == "true");
        } else if (key == "Type") {
            p.type = value;
            if (value == "Supporter")        p.cardType = 1;
            else if (value == "Item")        p.cardType = 2;
            else /* Pokémon */               p.cardType = 0;
        } else if (key == "Package") {
            p.package = value;
        } else if (key == "CanEvolve") {
            p.canEvolve = (toLower(value) == "true");
        } else if (key == "CardType") {
            p.cardType = parseIntOrZero(value); // Parse CardType.
        } else if (key == "HP") {
            p.hp = parseIntOrZero(value); // Parse HP.
        } else if (key == "Stage") {
            p.stage = parseIntOrZero(value); // Parse Stage.
            if (p.stage != 0) p.canEvolve = false;
        } else if (key == "Weakness") {
            p.weakness = (value == "None" ? "" : value);
        } else if (key == "RetreatCost") {
            p.retreatCost = parseIntOrZero(value); // Parse RetreatCost.
        } else if (key == "PrevEvo") {
            p.prevEvo = (value == "None" ? "" : value);
        } else if (key == "NextEvo") {
            p.nextEvo = (value == "None" ? "" : value);
        } else if (key == "Skills") {
            auto skillTokens = splitAndTrim(value, ';');
            for (auto &skillToken : skillTokens) {
                auto tokens = splitAndTrim(skillToken, ',');
                if (tokens.size() < 6) continue;
                Skill s(
                    tokens[0],
                    parseIntOrZero(tokens[1]),        // Damage.
                    parseIntOrZero(tokens[3]),        // Energy drop.
                    (toLower(tokens[4]) == "true"),   // Flip coin.
                    parseIntOrZero(tokens[5])         // Max flips.
                );
                // Parse energy requirements.
                for (auto &enToken : splitAndTrim(tokens[2], '|')) {
                    auto pos = enToken.find(':');
                    if (pos != std::string::npos) {
                        s.energyRequirements.emplace_back(
                            trim(enToken.substr(0, pos)),
                            parseIntOrZero(trim(enToken.substr(pos + 1)))
                        );
                    }
                }
                p.skills.push_back(std::move(s));
            }
        } else if (key == "SkillEffect") {
            if (p.skills.empty()) continue;
            auto &effect = p.skills.back().specialEffect;
            for (auto &tok : splitAndTrim(value, ';')) {
                applySkillEffectToken(trim(tok), effect);
            }
        } else if (key == "Abilities") {
            // Split multiple abilities by ';'
            for (auto &tok : splitAndTrim(value, ';')) {
                auto parts = splitAndTrim(tok, '|');
                if (parts.empty()) continue;
                // "None" means no ability
                if (toLower(parts[0]) == "none") 
                    continue;

                // parts[0] = ability name, parts[1] (if present) = description
                const std::string &name = parts[0];
                const std::string  desc = (parts.size() > 1 ? parts[1] : "");
                p.abilities.emplace_back(name, desc);
            }
        }
    }
    return false;
}

void processEnergyRequirements(std::vector<Skill> &skills) {
    #pragma omp parallel for
    for (size_t i = 0; i < skills.size(); ++i) {
        Skill &skill = skills[i];
        for (auto &req : skill.energyRequirements) {
            // Process each energy requirement
            if (req.amount <= 0) {
                std::cerr << "Warning: Invalid energy amount for skill: " << skill.skillName << std::endl;
            }
        }
    }
}

// Loads Pokémon card data from a file into the card map.
// Parameters:
// - filename: The file containing the Pokémon card data.
// - cardMap: The map to populate with the loaded Pokémon cards.
void loadCardMapFromFile(
    const std::string &filename,
    std::unordered_map<std::string, Pokemon> &cardMap
) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open card file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (trim(line) == "BEGIN_POKEMON") {
            Pokemon p;
            if (parsePokemonBlock(file, p)) {
                cardMap[normalize(p.name)] = p; // Normalize key to lowercase and remove spaces.
            }
        }
    }
}

// Loads a deck from a file into a vector of Pokémon.
// Parameters:
// - filename: The file containing the deck data.
// - deck: The vector to populate with the loaded deck.
// - cardMap: A map of card names to their corresponding Pokémon objects.
void loadDeckFromFile(const std::string &filename, std::vector<Pokemon> &deck, const std::unordered_map<std::string, Pokemon> &cardMap) {
    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::string cardName;
    while (std::getline(file, cardName)) {
        // Trim leading and trailing whitespace
        cardName = trim(cardName);
        std::cout << "Looking for card: " << cardName << std::endl; // Debugging line

        // Normalize the card name
        std::string normalizedCardName = normalize(cardName);
        auto it = cardMap.find(normalizedCardName); // Normalize input to lowercase
        if (it != cardMap.end()) {
            deck.push_back(it->second); // Add the card to the deck
        } else {
            std::cerr << "Warning: Card \"" << cardName << "\" not found in cardMap. Skipping.\n";
        }
    }

    // Close the file
    file.close();
}

// Loads the entire card database from a file into the card map.
// Parameters:
// - filename: The file containing the card database.
// - cardMap: The map to populate with the loaded card data.
void loadCardDatabase(const std::string &filename, std::unordered_map<std::string, Pokemon> &cardMap) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open card database file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == "BEGIN_POKEMON") {
            Pokemon p;
            if (parsePokemonBlock(file, p)) {
                cardMap[normalize(p.name)] = p; // Normalize key to lowercase and remove spaces
            }
        }
    }

    file.close();
}
