# Questions for AutoLisp Project

After reviewing `project.md` and `README.md`, I have the following questions to ensure the design and implementation meet expectations.

## 1. Scope and Features

1.  **Numeric Support:** The requirements only mention atoms, lists, and specific functions (`atom`, `eq`, `cons`, `car`, `cdr`, `quote`, `cond`, `lambda`). The examples use only symbolic atoms. Does the interpreter need to support integers or floating-point numbers, and if so, what arithmetic operations should be included?
2.  **String Support:** Are string literals (e.g., `"hello"`) supported as a distinct data type from symbols/atoms?
3.  **Scoping Rules:** The recursion examples (e.g., "Find first atom") appear to rely on **dynamic scoping** (where the function name `ff` passed as an argument is visible inside the body of the function when it is called). Can you confirm that the interpreter should implement dynamic scoping (Lisp 1.5 style) rather than lexical scoping (Scheme style)?
4.  **`label` Special Form:** `project.md` mentions `label` is not in the user-visible functions list in `README.md`, but it is a standard Lisp 1.5 mechanism for recursion. Should `label` be implemented as a special form, or should we rely solely on the Y-combinator-style recursion shown in the examples?
5.  **Additional Functions:** `project.md` lists internal design functions (`eval`, `apply`, `assoc`, `evcon`, `evlis`, `pairlis`). Are there any other user-facing standard library functions required (e.g., `null`, `list`, `caar`, `cadr`) beyond those listed in `README.md`?
6.  **Side Effects:** Should functions like `set`, `setq`, or `rplaca` be implemented? The current description implies a purely functional subset (except for the REPL loop itself).

## 2. Implementation Details

7.  **Garbage Collection Configuration:**
    *   You mentioned a "fixed-size allocation" for the heap and atoms. Should these sizes be hardcoded constants (e.g., `const int HEAP_SIZE = 1000000;`), or should they be defined in a way that's easily adjustable later?
    *   Is there a specific trigger policy for GC you prefer (e.g., "collect when allocation fails")?
    *   Should the GC implementation details (like stats) be exposed or hidden?
8.  **Atom Storage:** "Fixed-size space for atoms" implies a limit on the number of unique symbols. Should all atoms be interned (unique instances)?
9.  **Input Parsing:**
    *   What are the allowable characters for atoms? (e.g., only alphanumeric, or are symbols like `+`, `-`, `*` allowed?)
    *   Is the reader case-sensitive or case-insensitive?
    *   Should the parser handle improper lists (dotted pairs) in input, e.g., `(a . b)`?
10. **Environment Representation:** `project.md` mentions `pairlis`. Does this imply the environment should be implemented as an association list (alist)?
11. **Trace Format:** For the `--trace` option, is there a specific format required for the output? (e.g., indentation depth, printing arguments and return values).

## 3. Project Structure and Testing

12. **`doctest` Availability:** Since `doctest` is a header-only library, should I download `doctest.h` and include it in the repository, or assume it is available in the build environment?
13. **C++ Standard:** Which C++ standard should be targeted (C++11, C++14, C++17, C++20)?
14. **Directory Structure:** Are there preferences for folder structure (e.g., `src/`, `include/`, `test/`), or should files reside in the root/`lisp` directory?
15. **Build Artifacts:** The Makefile should produce an executable named `lisp`, correct?

## 4. Clarifications on Examples

16. **Pre-defined Atoms:** `README.md` lists `nil` and `t`. Are `nil` and `()` treated as the exact same object pointer in memory, or just semantically equivalent?
17. **Output Formatting:** How should dotted pairs be printed? For example, `(cons 'a 'b)` -> `(a . b)`. How about `(cons 'a (cons 'b 'c))` -> `(a b . c)` or `(a . (b . c))`?
