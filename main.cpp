#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "memory.h"
#include "read.h"
#include "print.h"
#include "eval.h"

void repl() {
    std::cout << "AutoLisp REPL\n";
    std::string buffer;
    while (true) {
        if (buffer.empty()) {
            std::cout << ">> ";
        } else {
            std::cout << ">>>> ";
        }

        std::string line;
        if (!std::getline(std::cin, line)) break;

        // If buffer is empty and line is empty, just skip
        if (buffer.empty() && line.empty()) continue;

        buffer += line + "\n";

        try {
            Cell* expr = read(buffer);
            if (!expr) {
                // Token stream was empty (whitespace/comments only)
                // Clear buffer to restart fresh
                buffer.clear();
                continue;
            }
            Cell* result = eval(expr, nil);
            std::cout << "=> " << print(result) << "\n";
            buffer.clear();
        } catch (const std::exception& e) {
            std::string msg = e.what();
            if (msg == "Unexpected EOF") {
                // Incomplete expression, continue reading
                continue;
            } else {
                std::cout << "Error: " << msg << "\n";
                buffer.clear();
            }
        }
    }
}

void run_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open file: " << filename << "\n";
        exit(1);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto tokens = tokenize(content);

    Cell* global_env = nil;

    while (!tokens.empty()) {
        try {
            Cell* expr = read_from_tokens(tokens);
            Cell* result = eval(expr, global_env);
            std::cout << print(result) << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            exit(1);
        }
    }
}

int main(int argc, char** argv) {
    init_memory();

    bool test_mode = false;
    std::string filename;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test") {
            test_mode = true;
        } else if (arg == "--trace") {
            gc_trace = true;
        } else {
            if (arg[0] != '-') {
                filename = arg;
            }
        }
    }

    if (test_mode) {
        doctest::Context context;
        context.applyCommandLine(argc, argv);
        return context.run();
    }

    if (!filename.empty()) {
        run_file(filename);
    } else {
        repl();
    }

    return 0;
}
