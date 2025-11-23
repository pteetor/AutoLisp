#pragma once
#include "memory.h"
#include <string>
#include <vector>

// Reads a single S-expression from the string.
Cell* read(const std::string& input);

// Tokenizes the input string.
std::vector<std::string> tokenize(const std::string& input);

// Reads a single S-expression from the token stream.
// Modifies the tokens vector by consuming tokens.
Cell* read_from_tokens(std::vector<std::string>& tokens);
