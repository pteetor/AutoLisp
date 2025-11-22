# AutoLisp Project

Your goal is to implement the program described in the file [README.md](README.md).

The program is an interpreter for the subset of Lisp described in the Lisp 1.5 Programmer's Manual
(hereafter called "the Manual").


## Project plan

1. Clarifing: You will read this file and the `README.md` file, then ask me any questions
   you have about this project so that I can give you more context. Be comprehensive.
   Write your questions to the file `questions.md` in Markdown format.
   Stop after you write the questions to the file, and then I will update
   this document and other documents to answer your questions.
   Do not write any code yet.
1. Designing: You will create a high level design for the program.
2. Planning: You will propose an implementation plan for the program. The plan will include
    - Coding
    - Debugging
    - Unit testing
    - Iintegration testing.
3. Coding: Code and test the program, iterating as needed to correct the code and improve the code.
4. Documenting: Describe the design.

## Design

Your design will likely follow the design of the "meta-circular" interpreter
included the Manual. That design was built on these functions (defined in the Manual).

- eval
- apply
- assoc
- evcon
- evlis
- pairlis

Every major function must capable of being tested. That is, it must be possible to write
a unit test for the function, and if not possible, the design must be reworked.

## Implementation

The interpreter will be written in C++. The C++ code can freely use the C++ standard libraries
for data structures and algorithms.

Include a gargage collector in the interpreter. Use a mark-and-sweep algorithm, so that circular
list structures will be reclaimed.

For now, assume the heap has a fixed-size allocation; for example, just assume it has room
for one million "cons" cells. Likewise, assume there is a fixed-size space for atoms.
(In the future, we may add command line options to let the user set these sizes.)

If the input sexpr causes an error, the interpreter should throw the error, report the error to the user,
and resume the REPL loop.

If the interpreter encounters a fatal error, it should report the error and halt. For example,
if memory is exhausted, even after garbage colletion, that is a fatal error.

Do not put all source code into one, large file.
Instead, break the source code into files, where each file contains a group of related functions.
Typical groupings could be

- main
- Parser for sexprs
- Evaluation of sexprs
- Heap management
- Utility functions

Create a Makefile that shows how to compile the program.

## Unit testing

Use the `doctest` framework for unit testing. At the end of each major C++ source file,
include unit tests for the important functions defined in that file.
