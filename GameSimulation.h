// GameSimulation.h
#ifndef GAMESIMULATION_H
#define GAMESIMULATION_H

#include "PokemonCard.h" // Includes GameState and related structures.
#include <unordered_map>
#include <string>

// Evaluates the game state and returns a value between 0.0 and 1.0.
// This function is used to assess the current state of the game.
// Parameters:
// - state: The current game state to evaluate.
// - depth: Optional parameter to specify the depth of evaluation (default is 0).
double evaluateGameState(const GameState &state, int depth = 0);

// Runs a Monte Carlo simulation over the game state.
// Simulates multiple random outcomes to estimate probabilities.
// Parameters:
// - state: The current game state.
// - numSimulations: The number of simulations to run.
// Returns:
// - A double representing the average outcome of the simulations.
double monteCarloSimulation(const GameState &state, int numSimulations);

// Processes input for the current round and updates the game state.
// Handles user interactions for actions like attacking, retreating, or using items.
// Parameters:
// - state: The current game state to update based on user input.
void processRoundInput(GameState &state);

// Loads a preset deck from the deck file into the game state.
// Validates the deck against the card database.
// Parameters:
// - deckFile: The file containing the deck information.
// - cardMap: A map of card names to their corresponding Pokémon objects.
// - deck: The vector to populate with the loaded deck.
void loadPresetDeck(const std::string &deckFile,
                    const std::unordered_map<std::string, Pokemon> &cardMap,
                    std::vector<Pokemon> &deck);

// Simulates drawing the initial hand into the game state.
// Ensures the player starts with a valid hand.
// Parameters:
// - state: The current game state to update with the initial hand.
void drawInitialHand(GameState &state);

// Recursively simulates decision tree outcomes up to a specified depth.
// Used to evaluate the best possible moves.
// Parameters:
// - state: The current game state.
// - depth: The depth of the decision tree to simulate.
// Returns:
// - A double representing the average outcome of the decision tree simulation.
double simulateDecisionTree(const GameState &state, int depth);

// Helper function to run the decision tree sequentially.
// Used internally for recursive simulations.
// Parameters:
// - state: The current game state.
// - depth: The depth of the decision tree to simulate.
// Returns:
// - A double representing the average outcome of the sequential simulation.
double simulateDecisionTreeSequential(const GameState &state, int depth);

// ----- Gameplay Phase Functions -----

// Pre-Start: Displays the current deck composition.
// Parameters:
// - state: The current game state containing the deck to display.
void preStartConfiguration(const GameState &state);

// Pre-1st Round: Collects coin flip result and opponent's main energy type.
// Parameters:
// - state: The current game state to update with pre-round configurations.
void preFirstRoundConfiguration(GameState &state);

// Post-1st Round: Collects board state updates and attack action data.
// Parameters:
// - state: The current game state to update.
// - cardMap: A map of card names to their corresponding Pokémon objects.
void postFirstRoundUpdate(GameState &state, const std::unordered_map<std::string, Pokemon> &cardMap);

// Pre-Every Round: Prompts for new card draw input.
// Parameters:
// - state: The current game state to update with the drawn card.
void preEveryRoundConfiguration(GameState &state);

// Post-Every Round: Processes action summary and updates the game state.
// Parameters:
// - state: The current game state to update with post-round actions.
void postEveryRoundUpdate(GameState &state);

#endif // GAMESIMULATION_H
