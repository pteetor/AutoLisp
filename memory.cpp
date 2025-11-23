#include "memory.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "doctest.h"

// Constants
const size_t HEAP_SIZE = 1000000;
Cell heap[HEAP_SIZE];
Cell* free_list = nullptr;
bool heap_initialized = false;

// Atom Table: Maps string name to the unique Symbol Cell
std::unordered_map<std::string, Cell*> atom_table;

// Globals
Cell* nil = nullptr;
Cell* truth = nullptr;
bool gc_trace = false;

// Internal allocation helper
Cell* alloc_raw() {
    if (!free_list) {
        return nullptr;
    }
    Cell* c = free_list;
    free_list = c->pair.cdr; // Move head to next

    // Reset cell state
    c->mark = false;
    // We don't clear type/data yet
    return c;
}

// Mark function for GC
void mark(Cell* c) {
    // If null or already marked, stop.
    // Note: Symbols are Cells too. If we mark a cons, we recurse.
    // If we mark a symbol, we just mark it and stop (symbols have no children).
    if (!c || c->mark) return;

    c->mark = true;

    if (c->type == Cell::CONS) {
        mark(c->pair.car);
        mark(c->pair.cdr);
    }
}

// Sweep function
void sweep() {
    size_t reclaimed = 0;
    size_t in_use = 0;

    // Rebuild free list from scratch?
    // Or traverse and prepend freed cells to free_list?
    // Traversing the whole heap is standard.

    // It's easier to rebuild the free list if we want to keep it simple,
    // but usually we just add dead cells to the existing free list.
    // Actually, since we sweep the WHOLE heap, we can just rebuild the free list
    // completely to ensure it's clean and maybe localized.
    // Let's reset free_list and rebuild it.

    free_list = nullptr;

    // Iterate from end to beginning to maintain some order, or beginning to end.
    for (size_t i = 0; i < HEAP_SIZE; ++i) {
        if (heap[i].mark) {
            heap[i].mark = false; // Reset for next time
            in_use++;
        } else {
            // It's garbage. Add to free list.
            // We treat the 'cdr' as the next pointer for the free list.
            // Note: We are corrupting the 'cdr' of the cell, but it's garbage so it's fine.
            heap[i].type = Cell::CONS; // Ensure we can interpret it as cons for free list? Not strictly needed if we just use pointer arithmetic or union, but good for safety.
            heap[i].pair.cdr = free_list;
            free_list = &heap[i];
            reclaimed++;
        }
    }

    if (gc_trace) {
        std::cout << "[GC] Reclaimed: " << reclaimed << ", In use: " << in_use << "\n";
    }
}

// Garbage Collection Entry Point
void gc(std::vector<Cell*> roots) {
    // Mark roots
    for (Cell* r : roots) {
        mark(r);
    }

    // Mark global constants
    mark(nil);
    mark(truth);

    // Mark all interned symbols?
    // If we want symbols to persist forever (as per fixed atom space implication),
    // we must mark them.
    for (auto& kv : atom_table) {
        mark(kv.second);
    }

    sweep();
}

Cell* cons(Cell* car, Cell* cdr) {
    if (!heap_initialized) init_memory();

    Cell* c = alloc_raw();
    if (!c) {
        // Attempt GC.
        // We protect car and cdr.
        gc({car, cdr});
        c = alloc_raw();
        if (!c) {
            std::cerr << "Fatal Error: Heap exhausted (cons).\n";
            exit(1);
        }
    }

    c->type = Cell::CONS;
    c->pair.car = car;
    c->pair.cdr = cdr;
    return c;
}

Cell* make_symbol(const std::string& name) {
    if (!heap_initialized) init_memory();

    // Check if already exists
    auto it = atom_table.find(name);
    if (it != atom_table.end()) {
        return it->second;
    }

    // Allocate new cell for symbol
    Cell* c = alloc_raw();
    if (!c) {
        gc({});
        c = alloc_raw();
        if (!c) {
             std::cerr << "Fatal Error: Heap exhausted (symbol).\n";
             exit(1);
        }
    }

    // We need to store the string name persistently.
    // The map key stores a copy of the string.
    // We can point to the key string in the map.
    // Insertion into unordered_map does not invalidate pointers to elements (C++17 standard for node-based maps, but check?)
    // std::unordered_map reference stability: "References and pointers to either key or data are invalidated only when the corresponding element is deleted."
    // Since we never delete symbols, this is safe.

    auto result = atom_table.emplace(name, c);
    const std::string* stored_name = &result.first->first;

    c->type = Cell::SYMBOL;
    c->symbol_name = stored_name;

    return c;
}

void init_memory() {
    if (heap_initialized) return;

    // Link up the free list
    for (size_t i = 0; i < HEAP_SIZE - 1; ++i) {
        heap[i].pair.cdr = &heap[i + 1];
    }
    heap[HEAP_SIZE - 1].pair.cdr = nullptr;
    free_list = &heap[0];

    heap_initialized = true;

    // Create constants
    // We must manually init nil and truth to avoid infinite recursion or checks if we used make_symbol inside init_memory logic differently.
    // But make_symbol calls init_memory if not initialized.
    // We set flag true first, so it's safe.

    nil = make_symbol("nil");
    truth = make_symbol("t");
}

bool is_symbol(Cell* c) {
    return c && c->type == Cell::SYMBOL;
}

bool is_cons(Cell* c) {
    return c && c->type == Cell::CONS;
}

// -----------------------------------------------------------------------------
// Unit Tests
// -----------------------------------------------------------------------------

TEST_CASE("Memory: Initialization and Symbols") {
    init_memory();
    CHECK(nil != nullptr);
    CHECK(truth != nullptr);
    CHECK(is_symbol(nil));
    CHECK(is_symbol(truth));
    CHECK(*nil->symbol_name == "nil");
    CHECK(*truth->symbol_name == "t");
}

TEST_CASE("Memory: Interning") {
    Cell* s1 = make_symbol("foo");
    Cell* s2 = make_symbol("foo");
    Cell* s3 = make_symbol("bar");

    CHECK(s1 == s2);
    CHECK(s1 != s3);
    CHECK(*s1->symbol_name == "foo");
}

TEST_CASE("Memory: Cons") {
    Cell* s1 = make_symbol("a");
    Cell* s2 = make_symbol("b");
    Cell* c = cons(s1, s2);

    CHECK(is_cons(c));
    CHECK(c->pair.car == s1);
    CHECK(c->pair.cdr == s2);
}

TEST_CASE("Memory: Garbage Collection") {
    // We need to simulate filling the heap.
    // Since heap is large (1M), we can't easily fill it in a quick test.
    // But we can verify that things are marked/swept if we manually trigger GC.

    gc_trace = false; // Silence output

    Cell* s1 = make_symbol("keep");
    Cell* c1 = cons(s1, nil);

    // c1 is a root.
    gc({c1});

    // c1 should still be valid.
    CHECK(c1->type == Cell::CONS);
    CHECK(c1->pair.car == s1);

    // Allocate something that we drop.
    Cell* garbage = cons(make_symbol("trash"), nil);
    (void)garbage; // Suppress unused warning

    // Trigger GC without including 'garbage' in roots.
    // 'garbage' should be reclaimed.
    // We can't easily check if it was reclaimed without inspecting the free list or using trace stats.
    // But we can check that c1 is still good.
    gc({c1});
    CHECK(c1->pair.car == s1);
}
