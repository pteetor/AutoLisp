#include "eval.h"
#include "memory.h"
#include "print.h" // For debugging
#include "read.h" // For tests
#include <stdexcept>
#include "doctest.h"

// Primitives
Cell* prim_car(Cell* args) {
    if (!is_cons(args) || args->pair.cdr != nil) throw std::runtime_error("car expects 1 argument");
    Cell* c = args->pair.car;
    if (!is_cons(c)) throw std::runtime_error("car expects a list");
    return c->pair.car;
}

Cell* prim_cdr(Cell* args) {
    if (!is_cons(args) || args->pair.cdr != nil) throw std::runtime_error("cdr expects 1 argument");
    Cell* c = args->pair.car;
    if (!is_cons(c)) throw std::runtime_error("cdr expects a list");
    return c->pair.cdr;
}

Cell* prim_cons(Cell* args) {
    // args should be (x y)
    if (!is_cons(args) || !is_cons(args->pair.cdr) || args->pair.cdr->pair.cdr != nil)
        throw std::runtime_error("cons expects 2 arguments");
    Cell* x = args->pair.car;
    Cell* y = args->pair.cdr->pair.car;
    return cons(x, y);
}

Cell* prim_atom(Cell* args) {
    if (!is_cons(args) || args->pair.cdr != nil) throw std::runtime_error("atom expects 1 argument");
    Cell* c = args->pair.car;
    if (is_symbol(c)) return truth;
    return nil; // In Lisp 1.5, (atom '()) is true because nil is an atom.
    // My implementation: nil is a SYMBOL, so it returns true. Correct.
    // Wait, is nil a symbol in my impl? Yes.
}

Cell* prim_eq(Cell* args) {
    if (!is_cons(args) || !is_cons(args->pair.cdr) || args->pair.cdr->pair.cdr != nil)
        throw std::runtime_error("eq expects 2 arguments");
    Cell* x = args->pair.car;
    Cell* y = args->pair.cdr->pair.car;

    if (is_symbol(x) && is_symbol(y)) {
        return (x == y) ? truth : nil;
    }
    return (x == y) ? truth : nil; // Default equality for atoms is pointer eq
}

Cell* prim_null(Cell* args) {
     if (!is_cons(args) || args->pair.cdr != nil) throw std::runtime_error("null expects 1 argument");
    Cell* c = args->pair.car;
    // (null x) is (eq x nil)
    return (c == nil) ? truth : nil;
}

// Environment Lookup
Cell* lookup(Cell* atom, Cell* env) {
    if (atom == truth) return truth;
    if (atom == nil) return nil;

    // Env is ((k . v) ...)
    Cell* curr = env;
    while (is_cons(curr)) {
        Cell* pair = curr->pair.car;
        if (is_cons(pair)) {
            if (pair->pair.car == atom) {
                return pair->pair.cdr;
            }
        }
        curr = curr->pair.cdr;
    }
    throw std::runtime_error("Unbound symbol: " + *atom->symbol_name);
}

// Eval List (helper for function application)
Cell* evlis(Cell* list, Cell* env) {
    if (list == nil) return nil;
    if (!is_cons(list)) throw std::runtime_error("evlis expected list");

    Cell* head = eval(list->pair.car, env);
    // We need to protect 'head' from GC if eval(rest) triggers it?
    // But 'head' is a result, so it should be safe if passed as root or stored in stack.
    // See previous discussion on GC roots. For now we assume safety or leak.

    Cell* tail = evlis(list->pair.cdr, env);
    return cons(head, tail);
}

Cell* eval(Cell* expr, Cell* env) {
    if (is_symbol(expr)) {
        return lookup(expr, env);
    }

    if (is_cons(expr)) {
        Cell* fn = expr->pair.car;
        Cell* args = expr->pair.cdr;

        // Special forms
        if (is_symbol(fn)) {
            std::string name = *fn->symbol_name;
            if (name == "quote") {
                if (!is_cons(args) || args->pair.cdr != nil) throw std::runtime_error("quote expects 1 argument");
                return args->pair.car;
            }
            if (name == "cond") {
                // (cond (p1 e1) (p2 e2) ...)
                Cell* curr = args;
                while (is_cons(curr)) {
                    Cell* clause = curr->pair.car;
                    if (!is_cons(clause) || !is_cons(clause->pair.cdr)) throw std::runtime_error("cond clause invalid");

                    Cell* pred = eval(clause->pair.car, env);
                    if (pred != nil) {
                        return eval(clause->pair.cdr->pair.car, env);
                    }
                    curr = curr->pair.cdr;
                }
                return nil; // Undefined? Or nil.
            }
        }

        // Function application
        Cell* eval_args = evlis(args, env);
        return apply(fn, eval_args, env);
    }

    throw std::runtime_error("Cannot eval: " + print(expr));
}

Cell* apply(Cell* fn, Cell* args, Cell* env) {
    if (is_symbol(fn)) {
        std::string name = *fn->symbol_name;
        if (name == "car") return prim_car(args);
        if (name == "cdr") return prim_cdr(args);
        if (name == "cons") return prim_cons(args);
        if (name == "eq") return prim_eq(args);
        if (name == "atom") return prim_atom(args);
        if (name == "null") return prim_null(args);

        // If not primitive, maybe it's a function in env?
        // But Lisp 1.5 usually has separate namespaces or uses properties.
        // Here, let's assume we lookup the function definition in env?
        // Or is the symbol itself bound to a lambda?
        // The prompt says "evaluate arguments (using evlis), then call apply".
        // And apply: "If fn is an atom (built-in primitive)... If fn is a lambda...".
        // It does NOT say lookup symbol in env.
        // BUT, if I have `(myfunc arg)`, `myfunc` is a symbol.
        // If `myfunc` is not a primitive, I should probably evaluate `myfunc` first?
        // "If expr is a list: ... Otherwise, it's a function application. Evaluate arguments ... then call apply."
        // The `fn` passed to `apply` is `expr->pair.car` which is UN-EVALUATED if it is a symbol.
        // But in Lisp 1.5, the car of the form IS the function.
        // If it is a symbol, it names a function.
        // We check primitives. If not primitive, we should look it up?
        // In Lisp 1.5 `(f x)`: `f` is looked up in the property list or environment.
        // Here we have dynamic scoping with alist.
        // So if `fn` is a symbol, we should look it up in `env`?
        // But `eval` logic says: "If expr is a list... Function application... apply(fn, eval_args, env)".
        // `fn` is passed as is.

        // So inside apply:
        // Check primitives.
        // If not primitive, lookup `fn` in `env`.
        // If result is a lambda/label, recurse `apply`.

        try {
            Cell* fn_def = lookup(fn, env);
            return apply(fn_def, args, env);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Undefined function: " + name);
        }
    }

    if (is_cons(fn)) {
        Cell* tag = fn->pair.car;
        if (is_symbol(tag)) {
            std::string name = *tag->symbol_name;
            if (name == "lambda") {
                // (lambda (params) body)
                // args are evaluated values.
                Cell* params = fn->pair.cdr->pair.car;
                Cell* body = fn->pair.cdr->pair.cdr->pair.car;

                // Bind args to params
                Cell* new_env = env; // Extend env
                // pairlis
                Cell* p = params;
                Cell* a = args;
                while (is_cons(p) && is_cons(a)) {
                    Cell* var = p->pair.car;
                    Cell* val = a->pair.car;
                    new_env = cons(cons(var, val), new_env);
                    p = p->pair.cdr;
                    a = a->pair.cdr;
                }
                if (p != nil || a != nil) throw std::runtime_error("Arity mismatch");

                return eval(body, new_env);
            }
            if (name == "label") {
                // (label name lambda)
                Cell* fname = fn->pair.cdr->pair.car;
                Cell* lambda = fn->pair.cdr->pair.cdr->pair.car;

                // Bind name to lambda in env, then apply lambda
                Cell* new_env = cons(cons(fname, lambda), env);
                return apply(lambda, args, new_env);
            }
        }
    }

    throw std::runtime_error("Invalid function to apply");
}

TEST_CASE("Evaluator: Primitives") {
    init_memory();
    Cell* env = nil;

    SUBCASE("Quote") {
        CHECK(print(eval(read("(quote a)"), env)) == "a");
        CHECK(print(eval(read("(quote (a b))"), env)) == "(a b)");
    }

    SUBCASE("Car/Cdr/Cons") {
        CHECK(print(eval(read("(car (quote (a b)))"), env)) == "a");
        CHECK(print(eval(read("(cdr (quote (a b)))"), env)) == "(b)");
        CHECK(print(eval(read("(cons (quote a) (quote b))"), env)) == "(a . b)");
    }

    SUBCASE("Eq/Atom/Null") {
        CHECK(eval(read("(eq (quote a) (quote a))"), env) == truth);
        CHECK(eval(read("(eq (quote a) (quote b))"), env) == nil);
        CHECK(eval(read("(atom (quote a))"), env) == truth);
        CHECK(eval(read("(atom (quote (a)))"), env) == nil);
        CHECK(eval(read("(null nil)"), env) == truth);
        CHECK(eval(read("(null (quote a))"), env) == nil);
    }

    SUBCASE("Cond") {
        CHECK(print(eval(read("(cond ((eq (quote a) (quote a)) (quote first)) (t (quote second)))"), env)) == "first");
        CHECK(print(eval(read("(cond ((eq (quote a) (quote b)) (quote first)) (t (quote second)))"), env)) == "second");
    }
}

TEST_CASE("Evaluator: Lambda") {
    init_memory();
    Cell* env = nil;

    // ((lambda (x) (cons x x)) (quote a))
    Cell* expr = read("((lambda (x) (cons x x)) (quote a))");
    CHECK(print(eval(expr, env)) == "(a . a)");
}

TEST_CASE("Evaluator: Label/Recursion") {
    init_memory();
    Cell* env = nil;

    // Define a recursive function 'append' using label
    // (label append (lambda (x y) (cond ((null x) y) (t (cons (car x) (append (cdr x) y))))))
    // We will call it: (append (quote (a b)) (quote (c d)))

    std::string code =
        "((label append (lambda (x y) "
        "   (cond ((null x) y) "
        "         (t (cons (car x) (append (cdr x) y)))))) "
        " (quote (a b)) (quote (c d)))";

    Cell* expr = read(code);
    Cell* result = eval(expr, env);
    CHECK(print(result) == "(a b c d)");
}
