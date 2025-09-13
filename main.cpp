#include <iostream>
#include <unordered_map>
#include "PokemonCard.h"
#include "FileParser.h"
#include "GameSimulation.h"
#include "Utils.h"

int main() {
    // Load the card map from the card file.
    std::unordered_map<std::string, Pokemon> cardMap;
    const std::string cardFile = "Cards.txt";
    loadCardMapFromFile(cardFile, cardMap);
    std::cout << "Total cards loaded from file: " << cardMap.size() << std::endl;

    // Initialize game state and load the preset deck.
    GameState state;
    const std::string deckFile = "deck.txt";  // Path to the player's deck file.
    loadPresetDeck(deckFile, cardMap, state.deck);
    preStartConfiguration(state);

    // Pre-1st Round: Initial configuration.
    preFirstRoundConfiguration(state);
    drawInitialHand(state);

    // Post-1st Round: Update board state after round one.
    postFirstRoundUpdate(state, cardMap);

    // Simulate multiple rounds.
    const int maxRounds = 5; // Number of rounds to simulate.
    for (int round = 2; round <= maxRounds; ++round) {
        std::cout << "\n===== Round " << round << " =====" << std::endl;

        // Pre-round: Draw phase.
        preEveryRoundConfiguration(state);

        // Process round actions based on user input.
        processRoundInput(state);

        // Post-round update: Update the game state after the round.
        postEveryRoundUpdate(state);

        // Simulate decision tree to evaluate the best move.
        int simulationDepth = 3; // Depth of the decision tree simulation.
        double winProbability = simulateDecisionTree(state, simulationDepth) * 100; // Convert to percentage.
        std::cout << "Winning probability for this round: " << winProbability << "%" << std::endl;

        // Increment the turn counter.
        state.turn++;
    }

    return 0;
}
