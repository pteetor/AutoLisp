#include "print.h"
#include "doctest.h"

std::string print(Cell* c) {
    if (c == nil) return "nil";
    if (c == truth) return "t"; // Although t is just a symbol, we might want to ensure it prints as t. But if it's a symbol, the symbol logic handles it.

    if (is_symbol(c)) {
        // c->symbol_name is a pointer to std::string
        return *c->symbol_name;
    }

    if (is_cons(c)) {
        std::string s = "(";
        Cell* curr = c;
        while (curr != nil) {
            if (!is_cons(curr)) {
                // Dotted pair
                s += ". ";
                s += print(curr);
                break;
            }

            s += print(curr->pair.car);
            curr = curr->pair.cdr;

            if (curr != nil) {
                s += " ";
            }
        }
        s += ")";
        return s;
    }

    return "?";
}

TEST_CASE("Printer") {
    init_memory();

    SUBCASE("Atoms") {
        CHECK(print(nil) == "nil");
        CHECK(print(truth) == "t");
        CHECK(print(make_symbol("foo")) == "foo");
    }

    SUBCASE("Lists") {
        Cell* l1 = cons(make_symbol("a"), nil);
        CHECK(print(l1) == "(a)");

        Cell* l2 = cons(make_symbol("b"), l1); // (b a)
        CHECK(print(l2) == "(b a)");

        Cell* l3 = cons(make_symbol("c"), cons(make_symbol("d"), nil));
        CHECK(print(l3) == "(c d)");
    }

    SUBCASE("Dotted Pairs") {
        Cell* d1 = cons(make_symbol("a"), make_symbol("b"));
        CHECK(print(d1) == "(a . b)");

        Cell* d2 = cons(make_symbol("a"), cons(make_symbol("b"), make_symbol("c")));
        CHECK(print(d2) == "(a b . c)");
    }
}
