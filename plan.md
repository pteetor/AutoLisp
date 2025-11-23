# Implementation Plan for AutoLisp

This plan outlines the steps to implement the AutoLisp interpreter. The project will be built in C++17, using a modular approach where each module handles a specific aspect of the interpreter (Memory, Reading, Printing, Evaluating).

## Phase 1: Infrastructure and Memory Management
**Goal:** Establish the build system and the core memory model. This is the foundation upon which the rest of the interpreter is built.

1.  **Build System Setup**
    *   Create a `Makefile` to compile the project (target `lisp`).
    *   Ensure `doctest.h` is available and linked correctly for tests.

2.  **Memory Module Implementation (`memory.h`, `memory.cpp`)**
    *   Define the `Cell` structure (tagged union for Symbol/Cons).
    *   Implement the fixed-size **Heap** (e.g., 1,000,000 cells).
    *   Implement the **Atom Table** (symbol interning) to ensure unique atoms.
    *   Implement `cons(car, cdr)` allocation logic.
    *   Implement `make_symbol(string)` logic.
    *   **Unit Testing**: Add `doctest` cases in `memory.cpp` to verify cell allocation, symbol uniqueness, and basic structure integrity.

3.  **Garbage Collector (Initial Implementation)**
    *   Implement the **Mark-and-Sweep** algorithm in `memory.cpp`.
    *   Implement the `--trace` flag logic to report stats (reclaimed/used).
    *   **Unit Testing**: Create tests that allocate enough cells to trigger GC and verify that unreferenced cells are reclaimed.

## Phase 2: Input/Output (Reader and Printer)
**Goal:** Enable the system to read S-expressions from text and print them back. This allows for early visual debugging.

1.  **Printer Implementation (`print.h`, `print.cpp`)**
    *   Implement recursive printing of `Cell` structures.
    *   Handle standard lists `(a b)` and dotted pairs `(a . b)`.
    *   **Unit Testing**: Test printing of atoms, simple lists, and nested lists.

2.  **Reader Implementation (`read.h`, `read.cpp`)**
    *   Implement a tokenizer to split input into `(`, `)`, `.`, and atoms.
    *   Implement a recursive descent parser to build `Cell` structures from tokens.
    *   Handle syntax errors (e.g., unbalanced parentheses).
    *   **Unit Testing**: Verify parsing of various S-expressions and ensure `read(print(x)) == x` (round-trip) concepts.

## Phase 3: Core Evaluator and Primitives
**Goal:** Implement the logic to evaluate expressions and the required primitive functions.

1.  **Evaluator Structure (`eval.h`, `eval.cpp`)**
    *   Define `eval(expr, env)` and `apply(fn, args, env)`.
    *   Implement environment lookups using association lists (alists).

2.  **Primitives Implementation**
    *   Implement `car`, `cdr`, `cons`, `eq`, `atom`, `null`.
    *   Implement `quote` as a special form in `eval`.
    *   **Unit Testing**: Add `doctest` cases in `eval.cpp` for each primitive.

## Phase 4: Special Forms and Control Flow
**Goal:** Implement the logic for functions, conditions, and recursion.

1.  **Special Forms**
    *   **`cond`**: Implement conditional logic.
    *   **`lambda`**: Implement function creation.
    *   **`label`**: Implement recursion support (binding names to lambdas).

2.  **Function Application**
    *   Update `apply` to handle user-defined functions (lambdas).
    *   Implement argument binding (extending the environment).

3.  **Unit Testing**
    *   Test `cond` with various branches.
    *   Test `lambda` definition and application.
    *   Test recursive functions (like factorial or length) in unit tests.

## Phase 5: REPL and Main Driver
**Goal:** Create the interactive executable.

1.  **Main Implementation (`main.cpp`)**
    *   Parse command-line arguments (`--trace`, file input).
    *   Implement the **REPL** (Read-Eval-Print Loop).
        *   Read expression.
        *   Evaluate expression in the global environment.
        *   Print result.
        *   Loop.
    *   Implement error handling: report errors and continue the loop (unless fatal).

## Phase 6: Integration Testing
**Goal:** Verify the complete system against external examples.

1.  **Test Scripting**
    *   Create a shell script `test_integration.sh`.
    *   The script will run the `lisp` executable against the files in the `lisp/` directory (e.g., `trivial.lisp`, `append.lisp`).
    *   Compare output with expected results (or manual verification if expected output isn't provided).

2.  **Final Verification**
    *   Run all unit tests (`./lisp --test` or similar mechanism via `doctest`).
    *   Run integration tests.
    *   Verify memory safety (using `--trace` or external tools if needed).

## Debugging Strategy
*   **Visual Debugging**: Use the `Printer` early on to inspect `Cell` structures.
*   **GC Tracing**: Use the `--trace` flag to debug memory leaks or exhaustion issues.
*   **Unit Tests**: Use `doctest` failures to isolate bugs in specific modules before integration.
