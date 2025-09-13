// GameSimulation.cpp
#include "GameSimulation.h"
#include "Constants.h"
#include "Utils.h"
#include "FileParser.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <limits>
#include <omp.h>
#include <fstream>
#include <cctype>
#include <sstream>
#include <unordered_set>

// Global variable to store all meta-decks at program startup.
std::vector<std::string> allMetaDecks;

// Loads all meta-decks from a file into the global `allMetaDecks` vector.
// Parameters:
// - filename: The file containing the meta-deck data.
void loadAllMetaDecks(const std::string &filename) {
    std::ifstream metaFile(filename);
    if (!metaFile.is_open()) {
        std::cerr << "Error: Could not open meta-decks file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string currentDeck;
    while (std::getline(metaFile, line)) {
        line = trim(line);
        if (line == "BEGIN_DECK") {
            currentDeck.clear();
        } else if (line == "END_DECK") {
            allMetaDecks.push_back(currentDeck);
        } else {
            currentDeck += line + "\n";
        }
    }
    metaFile.close();
}

// Filters meta-decks based on visible Pokémon on the opponent's board.
// Parameters:
// - visiblePokemons: A vector of visible Pokémon names.
// Returns:
// - A vector of filtered meta-decks matching the visible Pokémon.
std::vector<std::string> filterMetaDecksByVisibleBoard(
    const std::vector<std::string> &visiblePokemons
) {
    std::vector<std::string> filteredDecks;

    #pragma omp parallel
    {
        std::vector<std::string> localFilteredDecks; // Thread-local storage for matches.

        #pragma omp for schedule(dynamic)
        for (size_t i = 0; i < allMetaDecks.size(); ++i) {
            const auto &deck = allMetaDecks[i];
            std::istringstream deckStream(deck);
            std::unordered_set<std::string> deckPokemons; // Fixed: Added std::unordered_set
            std::string line;

            // Parse Pokémon names in the deck.
            while (std::getline(deckStream, line)) {
                line = trim(line);
                if (line.empty() || line == "BEGIN_DECK" || line == "END_DECK") continue;

                auto tokens = splitAndTrim(line, ',');
                if (tokens.size() == 2) {
                    deckPokemons.insert(toLower(tokens[0]));
                }
            }

            // Check if all visible Pokémon are in the deck.
            bool allMatch = true;
            for (const auto &visiblePoke : visiblePokemons) {
                if (deckPokemons.find(toLower(visiblePoke)) == deckPokemons.end()) {
                    allMatch = false;
                    break;
                }
            }

            if (allMatch) {
                localFilteredDecks.push_back(deck);
            }
        }

        // Merge thread-local results into the global vector.
        #pragma omp critical
        filteredDecks.insert(filteredDecks.end(), localFilteredDecks.begin(), localFilteredDecks.end());
    }

    if (filteredDecks.empty()) {
        return allMetaDecks; // Return all meta-decks if no matches are found.
    }

    return filteredDecks;
}

// Updates the opponent's meta-deck guesses based on visible Pokémon.
// Parameters:
// - state: The current game state to update with meta-deck guesses.
void updateMetaDeckGuesses(GameState &state) {
    std::vector<std::string> visiblePokemons;

    // Collect visible Pokémon names (active + bench).
    visiblePokemons.push_back(state.opponentActivePokemon.name);
    for (const auto &benchPoke : state.opponentBench) {
        visiblePokemons.push_back(benchPoke.name);
    }

    // Filter meta-decks based on visible Pokémon.
    state.oppMetaDeckGuesses = filterMetaDecksByVisibleBoard(visiblePokemons);
}

// Seeds the random number generator for parallel threads.
void seedRNG() {
    static bool seeded = false;
    if (!seeded) {
        #pragma omp parallel
        {
            thread_local std::random_device rd;
            thread_local std::mt19937 gen(rd());
        }
        seeded = true;
    }
}

// Evaluates the game state and returns a random value between 0.0 and 1.0.
// This is a placeholder for a more complex evaluation function.
// Parameters:
// - state: The current game state to evaluate.
// - depth: Optional parameter for evaluation depth (not used here).
// Returns:
// - A random double between 0.0 and 1.0.
double evaluateGameState(const GameState &state, int /*depth*/) {
    seedRNG(); // Ensure RNG is seeded once.
    thread_local std::mt19937 gen;
    std::uniform_real_distribution<> dis(0.0, 1.0);

    return dis(gen); // Return a random value between 0.0 and 1.0.
}

// Processes user input for the current round and updates the game state.
// Parameters:
// - state: The current game state to update based on user input.
void processRoundInput(GameState &state) {
    std::cout << "\nEnter round information (type 'exit' to terminate):" << std::endl;
    // ...existing code for processing user input...
}

// Loads a preset deck from the deck file into the game state.
void loadPresetDeck(
    const std::string &deckFile,
    const std::unordered_map<std::string, Pokemon> &cardMap,
    std::vector<Pokemon> &deck
) {
    std::ifstream file(deckFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open deck file: " << deckFile << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = splitAndTrim(line, ',');
        if (tokens.size() < 2) {
            std::cerr << "Warning: Deck entry malformed (need name, count): '"
                      << line << "'" << std::endl;
            continue;
        }

        const std::string cardName = tokens[0];
        int count = parseIntOrZero(tokens[1]);

        auto it = cardMap.find(normalize(cardName));
        if (it == cardMap.end()) {
            std::cerr << "Warning: Card '" << cardName
                      << "' not found in card database." << std::endl;
            continue;
        }

        for (int i = 0; i < count; ++i) {
            deck.push_back(it->second);
        }
    }

    file.close();
}

// Simulates drawing the initial hand into the game state.
// Parameters:
// - state: The current game state to update with the initial hand.
void drawInitialHand(GameState &state) {
    std::cout << "\nEnter your initial hand (5 cards, separated by commas): ";
    std::string input;
    std::getline(std::cin, input);

    auto cardNames = splitAndTrim(input, ',');
    if (cardNames.size() != INITIAL_HAND_SIZE) {
        std::cerr << "Error: You must select exactly " << INITIAL_HAND_SIZE << " cards for your initial hand." << std::endl;
        return;
    }

    for (const auto &cardName : cardNames) {
        auto it = std::find_if(state.deck.begin(), state.deck.end(),
                               [&cardName](const Pokemon &card) { return card.name == cardName; });
        if (it != state.deck.end()) {
            state.hand.push_back(*it);
            state.deck.erase(it);
        } else {
            std::cerr << "Error: Card '" << cardName << "' is not in your deck." << std::endl;
            return;
        }
    }
}

// Recursively simulates decision tree outcomes up to a specified depth.
// Parameters:
// - state: The current game state.
// - depth: The depth of the decision tree to simulate.
// Returns:
// - A double representing the average outcome of the decision tree simulation.
double simulateDecisionTree(const GameState &state, int depth) {
    if (depth == 0) {
        return evaluateGameState(state);
    }

    std::vector<GameState> nextStates;

    // Generate possible next states.
    const Pokemon &attacker = state.activePokemon;
    for (const auto &skill : attacker.skills) {
        GameState s = state;
        s.opponentActivePokemon.hp -= skill.dmg;
        if (s.opponentActivePokemon.hp < 0)
            s.opponentActivePokemon.hp = 0;

        nextStates.push_back(std::move(s));
    }

    if (nextStates.empty()) {
        return evaluateGameState(state);
    }

    double totalOutcome = 0.0;

    // Parallelize only the top level of recursion.
    #pragma omp parallel for reduction(+:totalOutcome) schedule(dynamic)
    for (size_t i = 0; i < nextStates.size(); ++i) {
        totalOutcome += simulateDecisionTreeSequential(nextStates[i], depth - 1);
    }

    return totalOutcome / nextStates.size();
}

// Helper function to run the decision tree sequentially.
// Parameters:
// - state: The current game state.
// - depth: The depth of the decision tree to simulate.
// Returns:
// - A double representing the average outcome of the sequential simulation.
double simulateDecisionTreeSequential(const GameState &state, int depth) {
    if (depth == 0) {
        return evaluateGameState(state);
    }

    std::vector<GameState> nextStates;

    // Generate possible next states.
    const Pokemon &attacker = state.activePokemon;
    for (const auto &skill : attacker.skills) {
        GameState s = state;
        s.opponentActivePokemon.hp -= skill.dmg;
        if (s.opponentActivePokemon.hp < 0)
            s.opponentActivePokemon.hp = 0;

        nextStates.push_back(std::move(s));
    }

    if (nextStates.empty()) {
        return evaluateGameState(state);
    }

    double totalOutcome = 0.0;

    for (const auto &nextState : nextStates) {
        totalOutcome += simulateDecisionTreeSequential(nextState, depth - 1);
    }

    return totalOutcome / nextStates.size();
}

// Pre-Start: Displays the current deck composition.
void preStartConfiguration(const GameState &state) {
    std::cout << "Pre-Start: Current Deck Composition:" << std::endl;
    for (const auto &card : state.deck) {
        std::cout << "  " << card.name << std::endl;
    }
}

// Pre-1st Round: Collects coin flip result and opponent's main energy type.
void preFirstRoundConfiguration(GameState &state) {
    std::string coinResult;
    std::cout << "\nPre-1st Round Configuration:" << std::endl;
    std::cout << "Enter coin flip result (H for Heads, T for Tails): ";
    std::cin >> coinResult;
    if (toLower(coinResult) == "h") {
        std::cout << "You will go first." << std::endl;
    } else {
        std::cout << "You will go second." << std::endl;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string opponentEnergy;
    std::cout << "Enter opponent's main energy type: ";
    std::getline(std::cin, opponentEnergy);
    std::cout << "Opponent's main energy type: " << opponentEnergy << std::endl;
}

// Post-1st Round: Collects board state updates and attack action data.
void postFirstRoundUpdate(GameState &state, const std::unordered_map<std::string, Pokemon> &cardMap) {
    std::cout << "\nPost-1st Round Update:\n";
    // ...existing code for updating the board state...
}

// Pre-Every Round: Prompts for new card draw input.
void preEveryRoundConfiguration(GameState &state) {
    std::cout << "\nPre-Every Round Configuration:\n";
    std::cout << "Enter the name of the drawn card: ";
    std::string drawnCard;
    std::getline(std::cin, drawnCard);

    if (!drawnCard.empty() && toLower(drawnCard) != "none") {
        state.hand.push_back(Pokemon(drawnCard));
    }
}

// Post-Every Round: Processes action summary and updates the game state.
void postEveryRoundUpdate(GameState &state) {
    std::cout << "\nPost-Every Round Update:\n";
    // ...existing code for processing actions and updating the state...
}
