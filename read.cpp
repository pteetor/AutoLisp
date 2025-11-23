#include "read.h"
#include "memory.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <cctype>
#include "doctest.h"

// Tokenizer
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;

    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == '(' || c == ')' || c == '.') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// Forward decl
Cell* read_list_body_fwd(std::vector<std::string>& tokens);

Cell* read_from_tokens(std::vector<std::string>& tokens) {
     if (tokens.empty()) {
        throw std::runtime_error("Unexpected EOF");
    }

    std::string token = tokens[0];
    tokens.erase(tokens.begin());

    if (token == "(") {
        return read_list_body_fwd(tokens);
    } else if (token == ")") {
        throw std::runtime_error("Unexpected ')'");
    } else if (token == ".") {
        throw std::runtime_error("Unexpected '.'");
    } else {
        return make_symbol(token);
    }
}

Cell* read_list_body_fwd(std::vector<std::string>& tokens) {
    if (tokens.empty()) throw std::runtime_error("Unexpected EOF");

    if (tokens[0] == ")") {
        tokens.erase(tokens.begin());
        return nil;
    }

    Cell* car = read_from_tokens(tokens);

    if (!tokens.empty() && tokens[0] == ".") {
        tokens.erase(tokens.begin());
        Cell* cdr = read_from_tokens(tokens);
        if (tokens.empty() || tokens[0] != ")") {
            throw std::runtime_error("Expected ')' after dotted pair");
        }
        tokens.erase(tokens.begin());
        return cons(car, cdr);
    }

    return cons(car, read_list_body_fwd(tokens));
}

Cell* read(const std::string& input) {
    if (!heap_initialized) init_memory();
    auto tokens = tokenize(input);
    if (tokens.empty()) return nil;
    return read_from_tokens(tokens);
}

// Tests
#include "print.h"

TEST_CASE("Reader") {
    init_memory();

    SUBCASE("Atoms") {
        CHECK(read("foo")->symbol_name != nullptr);
        CHECK(*read("foo")->symbol_name == "foo");
    }

    SUBCASE("Lists") {
        Cell* l = read("(a b)");
        CHECK(print(l) == "(a b)");
    }
}
