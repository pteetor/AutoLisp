# AutoLisp Project

Your goal is to implement the program described in the file [README.md](README.md).

The program is an interpreter for the subset of Lisp described in the Lisp 1.5 Programmer's Manual
(hereafter called "the Manual").


## Project plan
Follow this project plan. When writing document files (e.g., `design.md`),
use Markdown format.

1. Clarifing: You will read this file and the `README.md` file, then ask me any questions
   you have about this project so that I can give you more context. Be comprehensive.
   Write your questions to the file `questions.md`.
   Stop after you write the questions to the file, and then I will update
   this document and other documents to answer your questions.
   Do not write any code yet.
1. Designing: You will create a high level design for the program.
   Write the design to the new file `design.md`.
2. Planning: You will propose an implementation plan for the program.
   Write the plan to the new file `plan.md`.
   The plan will include
    - Coding
    - Debugging
    - Unit testing
    - Integration testing.
3. Coding: Code and test the program, iterating as needed to correct
   and improve the code.
4. Integration testing: Run the examples in the `examples` subdirectory.
   Confirm the they produce the expected result.

## Design

Your design will likely follow the design of the "meta-circular" interpreter
included the Manual. That design was built on these functions (defined in the Manual).

- `eval`
- `apply`
- related support functions, such as `assoc`, `evcon`, or `evlis`

For now, represent environments by association lists.
(In the future, we may use a more sophisticaed representation, to enable lexical scoping.)

Every major function must capable of being tested. That is, it must be possible to write
a unit test for the function, and if not possible, the design must be adjusted
so the unit testing is possible.

## Implementation

The interpreter will be written in C++, specifically the C++17 standard.
The C++ code can freely use the C++ standard libraries for data structures and algorithms.

Include a garbage collector in the interpreter. Use a mark-and-sweep algorithm, so that circular
list structures will be reclaimed.
Normally, the garbage collector is silent.
If the user specifies `--trace`, however, the garbage collector should report a few simple statistics
such as size of space reclaimed and size of space in use.

Intern all atoms so that instances are unique.

For now, assume the size of the heap is fixed and hard-coded into the program;
for example, you might fix the heap size at one million "cons" cells.
Likewise, assume there is a fixed-size space for atoms.
(In the future, we may add command line options to let the user set these sizes.)

If the input sexpr contans a syntax error, the interpreter should throw the error,
report the error to the user, and resume the REPL loop.

If the interpreter encounters a fatal error, it should report the error and halt.
For example, if either cons space or atom space are still exhausted
after garbage collection, that is a fatal error.

For the `--trace` option, just print something simple when `eval` is called;
for example, just the name of the function being evaluated (if the function is an atom).

## File organization

Do not put all source code into one, large file.
Instead, break the source code into files, where each file contains a group of related functions.
Typical groupings could be

- REPL code
- Parser for sexprs
- Evaluation of sexprs
- Heap management
- Utility functions

Create a Makefile that shows how to compile the program, which is called `lisp`.

For now, place all files in the root directory.
(Later, we may reorganize the files.)

## Unit testing

Use the `doctest` framework for unit testing. At the end of each major C++ source file,
include unit tests for the important functions defined in that file.
Note that the required `doctest.h` header file is already in the repository.

## Integration testing
The subdirectory `examples` contains several examples of Lisp programs
which should work correctly.
Use them to write a shell script which runs those examples.
