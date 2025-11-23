#pragma once
#include <string>
#include <vector>

struct Cell {
    enum Type { SYMBOL, CONS };
    Type type;

    union {
        const std::string* symbol_name;
        struct {
            Cell* car;
            Cell* cdr;
        } pair;
    };

    bool mark = false;
};

// Global constants
extern Cell* nil;
extern Cell* truth;

// Initialization
void init_memory();

// Allocation
Cell* cons(Cell* car, Cell* cdr);
Cell* make_symbol(const std::string& name);

// Predicates
bool is_symbol(Cell* c);
bool is_cons(Cell* c);

// Garbage Collection
extern bool gc_trace;
extern bool heap_initialized;

// gc takes root pointers. For now, we'll expose a function to register roots or just pass them.
// A simple way is to pass the environment and maybe a list of temporary roots.
void gc(std::vector<Cell*> roots);
