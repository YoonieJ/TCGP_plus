// PokemonCard.h
#ifndef POKEMONCARD_H
#define POKEMONCARD_H

#include <string>
#include <vector>

// Forward declarations
struct Skill;
struct Ability;

// --- Energy and Skill Structures ---

// Represents the energy type and amount required for a skill.
struct EnergyRequirement {
    std::string energyType;  // e.g., "Grass", "Fire", "Any"
    int amount;              // Amount of energy required.

    EnergyRequirement(const std::string &etype = "", int amt = 0)
        : energyType(etype), amount(amt) {}
};

// Specifies additional effects that a skill can have during an attack.
struct SpecialSkill {
    // --- Coin flip settings ---
    bool doCoinFlips = false;       // Whether coin flips occur.
    int numFlips = 0;               // Number of flips to attempt.
    bool flipUntilTails = false;    // Flip until tails (up to MAX_FLIP).

    // --- Flip-dependent damage ---
    int damagePerFlip = 0;          // Damage per heads when flipping until tails.
    int randomHitDamage = 0;        // Damage per hit in random multi-hit.
    int randomHitCount = 0;         // Number of random hits.

    // --- Basic effects ---
    int extraDmg = 0;               // Extra damage dealt unconditionally.
    int extraDmgIfPoisoned = 0;     // Extra damage if the target is poisoned.
    int extraDmgIfParalyzed = 0;    // Extra damage if the target is paralyzed.
    int damagePerEnergyAttached = 0; // +X damage per energy attached to the opponent's Active Pokémon.

    int heal = 0;                   // Amount to heal.
    int damageReduction = 0;        // Flat damage reduction.
    int benchedDamage = 0;          // Damage to each benched Pokémon.
    int numBenched = 0;             // Number of benched Pokémon to target.

    // --- Status and board effects ---
    bool switchOutOpp = false;      // Switch opponent’s Active Pokémon with a benched one.
    bool paralyzeOpp = false;       // Paralyze opponent’s Active Pokémon.
    bool poisonOpp = false;         // Poison opponent’s Active Pokémon.
    bool shuffleOpponentBackIfHeads = false; // Shuffle opponent's Active Pokémon back into the deck if heads.
    bool banSupporter = false;      // Ban opponent from using supporter cards next turn.

    // Constructor: Initializes all fields with default values.
    SpecialSkill(
        bool coinFlips = false,
        int flips = 0,
        bool flipTails = false,
        int dmgPerFlipArg = 0,
        int randHits = 0,
        int randHitDmgArg = 0,
        int xtra = 0,
        int xtraIfPoison = 0,
        int xtraIfPara = 0,
        int dmgPerEnergy = 0,
        int healAmt = 0,
        int dmgRed = 0,
        int benchDmg = 0,
        int benchCount = 0,
        bool swOpp = false,
        bool paraOpp = false,
        bool poisOpp = false,
        bool shuffleBack = false,
        bool banSupp = false
    )
      : doCoinFlips(coinFlips)
      , numFlips(flips)
      , flipUntilTails(flipTails)
      , damagePerFlip(dmgPerFlipArg)
      , randomHitCount(randHits)
      , randomHitDamage(randHitDmgArg)
      , extraDmg(xtra)
      , extraDmgIfPoisoned(xtraIfPoison)
      , extraDmgIfParalyzed(xtraIfPara)
      , damagePerEnergyAttached(dmgPerEnergy)
      , heal(healAmt)
      , damageReduction(dmgRed)
      , benchedDamage(benchDmg)
      , numBenched(benchCount)
      , switchOutOpp(swOpp)
      , paralyzeOpp(paraOpp)
      , poisonOpp(poisOpp)
      , shuffleOpponentBackIfHeads(shuffleBack)
      , banSupporter(banSupp)
    {}
};

// Represents a Pokémon's inherent passive ability.
struct Ability {
    bool activeOnly = false;        // Only active if the Pokémon is in the Active spot.
    bool poisonOpp = false;         // May poison the opponent's Active Pokémon.
    bool forceSwitchOpp = false;    // May force the opponent to switch their Active Pokémon.
    bool banSupporter = false;      // Prevent the opponent from using supporter cards next turn.
    bool moveEnergy = false;        // Move energy from bench/zone to Active Pokémon.

    int attachEnergyCount = 0;      // Number of energy cards to attach.
    std::string attachEnergyType;   // Type of energy to attach.
    bool attachOnlyActive = false;  // Only attach energy if the Pokémon is Active.

    std::string name;               // Name of the ability.
    std::string description;        // Description of the ability.

    // Default constructor
    Ability(
        bool _activeOnly = false,
        bool _poisonOpp = false,
        bool _forceSwitchOpp = false,
        bool _banSupporter = false,
        bool _moveEnergy = false,
        int _attachCount = 0,
        const std::string &_attachType = "",
        bool _attachOnlyActive = false
    )
      : activeOnly(_activeOnly)
      , poisonOpp(_poisonOpp)
      , forceSwitchOpp(_forceSwitchOpp)
      , banSupporter(_banSupporter)
      , moveEnergy(_moveEnergy)
      , attachEnergyCount(_attachCount)
      , attachEnergyType(_attachType)
      , attachOnlyActive(_attachOnlyActive)
    {}

    // Constructor for name and description
    Ability(const std::string &_name, const std::string &_description)
      : name(_name), description(_description) {}
};

// Represents an attack move.
struct Skill {
    std::string skillName;                     // Name of the skill.
    int dmg;                                   // Base damage of the skill.
    std::vector<EnergyRequirement> energyRequirements; // Energy requirements for the skill.
    int energyDrop;                            // Energy cost deducted when using the skill.
    bool flipCoin;                             // Whether the skill requires a coin flip.
    int maxFlip;                               // Maximum number of coin flips allowed.
    SpecialSkill specialEffect;                // Special effects associated with the skill.

    Skill(const std::string &name = "", int damage = 0, int energyDrop = 0,
          bool coin = false, int flipMax = 0)
        : skillName(name)
        , dmg(damage)
        , energyDrop(energyDrop)
        , flipCoin(coin)
        , maxFlip(flipMax)
        , specialEffect() {}
};

// --- Pokémon Card and Game State Structures ---

// Represents a card, which can be a Pokémon, Supporter, or Item.
struct Pokemon {
    std::string name;                          // Name of the card.
    bool isEx;                                 // Whether the card is an EX Pokémon.
    std::string type;                          // Type of the Pokémon (e.g., Grass, Fire).
    std::string package;                       // Card set or package.
    bool canEvolve;                            // Whether the Pokémon can evolve.
    int cardType;                              // 0: Pokémon, 1: Supporter, 2: Item.

    int hp;                                    // Hit points of the Pokémon.
    int stage;                                 // Evolution stage (0: Basic, 1: Stage 1, 2: Stage 2).
    std::string prevEvo;                       // Name of the previous evolution.
    std::string nextEvo;                       // Name of the next evolution.
    Pokemon* preEvolution;                    // Pointer to the previous evolution.
    Pokemon* postEvolution;                   // Pointer to the next evolution.

    std::vector<Skill> skills;                 // List of skills the Pokémon can use.
    std::vector<Ability> abilities;            // List of passive abilities.
    std::string weakness;                      // Weakness type (e.g., Fire).
    int retreatCost;                           // Energy cost to retreat the Pokémon.

    std::vector<EnergyRequirement> attachedEnergy; // Energy cards attached to the Pokémon.
    bool isPoisoned;                           // Whether the Pokémon is poisoned.
    bool isParalyzed;                          // Whether the Pokémon is paralyzed.

    Pokemon(const std::string &n = "", bool ex = false,
            const std::string &t = "", const std::string &pkg = "",
            bool evolve = false, int cType = 0,
            int health = 0, int stg = 0,
            const std::string &weak = "")
      : name(n), isEx(ex), type(t), package(pkg), canEvolve(evolve), cardType(cType),
        hp(health), stage(stg), prevEvo(""), nextEvo(""),
        preEvolution(nullptr), postEvolution(nullptr),
        skills(), abilities(), weakness(weak), retreatCost(0),
        attachedEnergy(), isPoisoned(false), isParalyzed(false)
    {}
};

// Represents an attack action during a game.
struct AttackRecord {
    std::string attacker;                      // Name of the attacking Pokémon.
    std::string moveName;                      // Name of the move used.
    std::string target;                        // Name of the target Pokémon.
    std::vector<std::string> effects;          // Effects of the attack (e.g., "PoisonOpp", "+30 dmg").
};

// Represents energy attachments during a game.
struct EnergyAttachment {
    std::string pokemonName;                   // Name of the Pokémon receiving the energy.
    std::string energyType;                    // Type of energy attached.
    int amount;                                // Amount of energy attached.
};

// Represents the current game state.
struct GameState {
    std::vector<Pokemon> deck;                 // Player's deck of cards.
    std::vector<Pokemon> hand;                 // Player's hand of cards.
    Pokemon activePokemon;                     // Player's active Pokémon.
    std::vector<Pokemon> bench;                // Player's benched Pokémon.
    int turn;                                  // Current turn number.
    bool firstTurn;                            // Whether it is the first turn.
    Pokemon opponentActivePokemon;             // Opponent's active Pokémon.
    std::vector<Pokemon> opponentBench;        // Opponent's benched Pokémon.
    std::vector<std::string> actionHistory;    // History of actions taken.

    std::vector<AttackRecord> attacksThisRound; // List of attacks performed this round.
    std::vector<EnergyAttachment> yourAttachments; // Energy attachments on the player's side.
    std::vector<EnergyAttachment> oppAttachments;  // Energy attachments on the opponent's side.
    std::vector<std::string> oppMetaDeckGuesses;   // Guesses for the opponent's meta-deck.

    GameState() : turn(0), firstTurn(true) {}
};

#endif // POKEMONCARD_H
