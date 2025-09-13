// Utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

// Removes leading and trailing whitespace from a string.
inline std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// Splits a string by the given delimiter and trims each token.
inline std::vector<std::string> splitAndTrim(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// Converts a string to lowercase for case-insensitive comparisons.
inline std::string toLower(const std::string &s) {
    std::string lowerStr = s;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Normalizes a string by converting it to lowercase and removing spaces.
inline std::string normalize(const std::string &s) {
    std::string normalized = toLower(s);
    normalized.erase(std::remove_if(normalized.begin(), normalized.end(), ::isspace), normalized.end());
    return normalized;
}

#endif // UTILS_H
