# Design Document for AutoLisp

This document outlines the high-level design for the AutoLisp interpreter, a subset of Lisp 1.5.

## 1. System Architecture

The interpreter will be organized into the following logical modules:

1.  **Main/REPL**: Handles command-line arguments, initialization, and the Read-Eval-Print Loop.
2.  **Reader (Parser)**: Parses input text (S-expressions) into internal data structures.
3.  **Evaluator**: Implements the core Lisp logic (`eval`, `apply`) and special forms.
4.  **Printer**: Converts internal data structures back into text format for display.
5.  **Memory Management**: Manages the heap, symbol table (interned atoms), and Garbage Collection (Mark-and-Sweep).

These modules will be implemented in C++17.

## 2. Data Structures

The core data structure is the `Cell`, which represents a Lisp object. Given the requirement for a fixed-size heap, we will use a pool of `Cell` objects.

### 2.1. Cell

A `Cell` is a tagged union (or equivalent C++ structure) that can represent:
*   **Symbol** (Atom): Represents a name (e.g., `quote`, `x`, `nil`).
*   **Cons**: A pair of two other Cells (`car` and `cdr`).

Implementation approach:
```cpp
struct Cell {
    enum Type { SYMBOL, CONS };
    Type type;
    union {
        // For SYMBOL: index into the Atom Table or pointer to string
        const std::string* symbol_name;

        // For CONS: pointers (or indices) to other Cells
        struct {
            Cell* car;
            Cell* cdr;
        } pair;
    } data;

    bool mark_bit = false; // For Garbage Collection
};
```
*Note: Since `nil` is equivalent to `()`, it will be a special Symbol. Pointers will be used for simpler C++ implementation, managed by the Heap.*

### 2.2. Environment

The environment will be represented as an **Association List (alist)**, as per `project.md`.
*   An alist is a list of pairs: `((symbol . value) (symbol . value) ...)`
*   This structure supports the required Dynamic Scoping.
*   Functions like `assoc` will look up values.
*   `pairlis` (or equivalent logic) will bind arguments to parameters.

## 3. Module Design

### 3.1. Memory Management (`memory.h`, `memory.cpp`)

*   **Heap**: A fixed-size array (or `std::vector`) of `Cell` objects to serve as the pool for Cons cells.
*   **Atom Table**: A `std::unordered_set<std::string>` or similar structure to store the actual string data of atoms.
*   **Interning**: When a symbol is read, we check the Atom Table. If it exists, we return a pointer to the existing string. If not, we add it. This ensures unique `symbol_name` pointers for fast `eq` comparisons.
*   **Allocation**: `cons(x, y)` requests a cell from the free list.
*   **Garbage Collection (GC)**:
    *   **Algorithm**: Mark-and-Sweep.
    *   **Roots**: The current environment, the expression currently being evaluated, and any temporary registers holding `Cell*`.
    *   **Trace**: If `--trace` is enabled, GC prints statistics (reclaimed count, in-use count).
    *   **Failure**: If heap is full after GC, the program halts with a fatal error.

### 3.2. Reader (`read.h`, `read.cpp`)

*   **Tokenizer**: Splits input stream into tokens: `(`, `)`, `.`, and `atoms`.
*   **Parser**: Recursive descent parser.
    *   `read()`: Reads one S-expression.
    *   Handles lists `(a b c)`, dotted pairs `(a . b)`, and standard atom syntax.
    *   Throws exceptions for syntax errors (e.g., unbalanced parens).

### 3.3. Evaluator (`eval.h`, `eval.cpp`)

Implements the Lisp 1.5 `eval` and `apply` logic.

*   **`eval(expr, env)`**:
    *   If `expr` is an atom, look it up in `env`. (If not found, error? Or return self? Lisp 1.5 usually expects bound variables. `t` and `nil` evaluate to themselves).
    *   If `expr` is a list:
        *   Check `car` for special forms: `quote`, `cond`, `lambda`, `label`.
        *   Otherwise, it's a function application. Evaluate arguments (using `evlis`), then call `apply`.
*   **`apply(fn, args, env)`**:
    *   If `fn` is an atom (built-in primitive):
        *   `car`, `cdr`, `cons`, `eq`, `atom`, `null`: Execute C++ implementation.
    *   If `fn` is a `lambda` expression `(lambda (params) body)`:
        *   Bind `args` to `params` (extend `env`).
        *   `eval(body, new_env)`.
    *   If `fn` is a `label` expression `(label name lambda)`:
        *   Bind `name` to the `lambda` in `env`.
        *   `apply(lambda, args, new_env)`.
    *   Else: Error.

### 3.4. Printer (`print.h`, `print.cpp`)

*   **`print(cell)`**:
    *   If Symbol: print name.
    *   If Cons: print `(` then iterate:
        *   Print `car`.
        *   If `cdr` is `nil`, stop.
        *   If `cdr` is Atom, print ` . ` then Atom.
        *   If `cdr` is Cons, print space and loop.
        *   Print `)`.

### 3.5. Main / REPL (`main.cpp`)

*   Parse command line (`--trace`, `file`).
*   Loop:
    1.  Print prompt `>> ` (if stdin).
    2.  `read()` expr.
    3.  `eval(expr, global_env)`.
    4.  Print `=> ` followed by `print(result)`.
*   Error Handling: Catch exceptions, print error message, continue loop (unless fatal memory error).

## 4. Testing Plan

*   **Unit Tests**: Used `doctest.h`.
    *   Test `cons`, `car`, `cdr` logic.
    *   Test parser with various inputs.
    *   Test `eval` with primitives.
    *   Test `lambda` and recursion.
    *   Test GC by filling the heap and verifying reclamation.
*   **Integration Tests**: Run the provided examples in `lisp/` directory and verify output.

## 5. Globals and Constants

*   `nil` and `t` will be globally accessible `Cell*` constants (or atoms).
*   Heap size and Atom space size will be defined as constants.

## 6. Simplifications & Assumptions

*   No numbers or strings (as per spec).
*   No `set` or side effects (purely functional).
*   Dynamic scoping simplifies the environment to a single list passed recursively.
