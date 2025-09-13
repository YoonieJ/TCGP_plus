// FileParser.h
#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <unordered_map>
#include <string>
#include "PokemonCard.h"
#include "Utils.h" // Include Utils.h to use normalize and other utility functions

// Loads Pokémon card data from a file into the cardMap.
void loadCardMapFromFile(const std::string &filename, std::unordered_map<std::string, Pokemon> &cardMap);
int parseIntOrZero(const std::string &raw);

#endif // FILEPARSER_H
