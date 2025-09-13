# TCG-P+: Pokémon TCG Probability Analyzer

---

## **Table of Contents**
1. [Overview](#overview)
2. [Features](#features)
3. [Installation](#installation)
4. [How to Use](#how-to-use)
   - [Pre-Start](#pre-start)
   - [During Gameplay](#during-gameplay)
   - [Post-Round](#post-round)
5. [Rules and Gameplay Mechanics](#rules-and-gameplay-mechanics)
   - [Deck Composition](#deck-composition)
   - [Turn Structure](#turn-structure)
   - [Victory Conditions](#victory-conditions)
6. [Technical Details](#technical-details)
   - [Parallel Computing with OpenMP](#parallel-computing-with-openmp)
   - [Data Structures](#data-structures)
7. [File Descriptions](#file-descriptions)
8. [References](#references)

---

## **Overview**

TCG-P+ is a C++-based probability analyzer designed to assist players of the Pokémon Trading Card Game Pocket (TCG-P) in making optimal in-game decisions. By leveraging parallel computing with OpenMP, TCG-P+ simulates and evaluates potential move outcomes in real time, providing actionable insights for players. The system is particularly useful for novice players facing the steep learning curve and time pressure of TCG-P's fast-paced gameplay.

---

## **Features**

1. **Deck Input Module**:
   - Load and validate a 20-card deck from a file.
   - Supports up to 2 copies of the same card.

2. **Game State Analyzer**:
   - Input and track the current game state, including:
     - Active and benched Pokémon.
     - Energy attachments.
     - Opponent's visible board state.

3. **Recommendation Engine**:
   - Provides real-time suggestions for:
     - Optimal moves.
     - Energy placements.
     - Card usage.

4. **Parallelized Simulations**:
   - Uses OpenMP to accelerate:
     - Recursive decision tree evaluations.
     - Monte Carlo simulations for probabilistic outcomes.

5. **Meta-Deck Estimation**:
   - Automatically identifies potential opponent decks based on visible Pokémon and energy types.

---

## **Installation**

### **Language and Parallelism**
This project is written in **C++**. I chose OpenMP to leverage multi-core parallelism for fast Monte Carlo simulations and recursive decision-tree evaluations, ensuring real-time probabilistic insights during gameplay. Its simple pragma-based approach integrates seamlessly into my existing C++ code without adding complex thread management.

### **Setup**
1. Clone or download the project files.
2. Ensure the following files are present:
   - `main.cpp`
   - `PokemonCard.h`
   - `GameSimulation.h` and `GameSimulation.cpp`
   - `FileParser.h` and `FileParser.cpp`
   - `Utils.h`
   - `Cards.txt` (card database)
   - `deck.txt` (your deck)
   - `metaDecks.txt` (meta-deck data)

---

## **How to Use**

### **Pre-Start**
1. Prepare a 20-card deck in `deck.txt`.
2. Ensure `Cards.txt` contains all card definitions.
3. Run the program and follow the prompts to:
   - Load your deck.
   - View your starting hand.
   - Set up the initial board state.

### **During Gameplay**
1. Input the current game state after each round:
   - Active and benched Pokémon.
   - Energy attachments.
   - Attack actions and effects.
2. Receive recommendations for:
   - Optimal moves.
   - Energy placements.
   - Probabilistic insights into winning outcomes.

### **Post-Round**
1. Update the board state and log actions.
2. View estimated opponent meta-decks based on their visible board.

---

## **Rules and Gameplay Mechanics**

### **Deck Composition**
- 20 cards per deck.
- Up to 2 copies of the same card.

### **Turn Structure**
1. **Draw Phase**: Draw one card from the deck.
2. **Action Phase**:
   - Attach energy.
   - Play supporter and item cards.
   - Evolve Pokémon.
   - Retreat Pokémon.
   - Use abilities.
3. **Attack Phase**: Use one attack from the active Pokémon.

### **Victory Conditions**
- Earn 3 points to win:
  - 1 point for defeating a regular Pokémon.
  - 2 points for defeating an EX Pokémon.

---

## **Technical Details**

### **Parallel Computing with OpenMP**

OpenMP is used extensively in this project to accelerate computationally intensive tasks, ensuring real-time performance during gameplay. Key areas where OpenMP is applied include:

1. **Recursive Decision Tree Evaluation**:
   - Simulates potential move sequences and outcomes.
   - Parallelized to explore multiple branches of the decision tree simultaneously.
   - Each branch is evaluated independently, making it ideal for multi-threading.

2. **Monte Carlo Simulations**:
   - Estimates probabilities for card draws and move successes.
   - Distributes iterations across threads for faster results.
   - OpenMP's dynamic scheduling ensures efficient load balancing across cores.

3. **Thread-Safe Data Management**:
   - Shared memory is used for caching game states.
   - Critical sections and atomic operations ensure thread safety during updates.

By leveraging OpenMP, the program achieves significant speedups, enabling it to provide actionable insights within seconds, even for complex game states.

### **Data Structures**
1. **Card Database**:
   - Stored as an `std::unordered_map<std::string, Pokemon>`.
   - Provides fast lookups for card attributes.

2. **Game State**:
   - Tracks the current board, hand, and deck.
   - Includes energy attachments, attack actions, and meta-deck guesses.

3. **Decision Trees**:
   - Represents possible move sequences and outcomes.
   - Evaluated recursively with parallelization.

---

## **File Descriptions**

1. **Source Code**:
   - `main.cpp`: Entry point for the program.
   - `GameSimulation.cpp` and `GameSimulation.h`: Core game logic and simulations.
   - `FileParser.cpp` and `FileParser.h`: File parsing utilities for cards and decks.
   - `Utils.h`: Helper functions for string manipulation.

2. **Data Files**:
   - `Cards.txt`: Database of all Pokémon, supporter, and item cards.
   - `deck.txt`: Your 20-card deck.
   - `metaDecks.txt`: Predefined meta-decks for opponent estimation.

3. **Code Structure**:
   - **`PokemonCard.h`**: Defines the core data structures for Pokémon, skills, abilities, and game state.
   - **`GameSimulation.cpp`**: Implements gameplay logic, including decision tree simulations and meta-deck estimation.
   - **`FileParser.cpp`**: Handles file input for loading cards and decks.
   - **`Utils.h`**: Provides utility functions for string manipulation and parsing.

4. **Gameplay Flow**:
   - **Pre-Start**: Load deck and initialize the game state.
   - **Round Updates**: Input board state, energy attachments, and attack actions.
   - **Simulations**: Evaluate potential moves and calculate winning probabilities.

---

## **References**

1. [Game8 Pokémon TCG Guide](https://game8.co/games/Pokemon-TCG-Pocket/archives/483063)
2. [Bulbapedia: Pokémon TCG Types](https://bulbapedia.bulbagarden.net/wiki/Type_(TCG))
