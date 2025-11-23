# Lisp

This is an interpreter for the subset of Lisp described in the Lisp 1.5 Programmer's Manual.
The program executes a simple REPL loop, reading a Lisp S-expression ("sexpr") from stdin or a file,
evaluating the sexpr, and printing the result, then repeating until end-of-file.

Usage: `lisp [--trace] [file]`

Options:

- `--trace`     Trace calls to eval
- `file`        Read input from file instead of stdin

When reading from stdin, the program prompts the user with the string ">>".
If the sexpr extends over multiple lines, the program reads input lines until it reaches the end of the sexpr.
When reading continuation lines, the program prompts the user with the string ">>>>".
When printing the result of evaluation, the program first prints "=>".

## The Lisp dialect

### Input syntax

The intepreter expects the usual, simple syntax for Lisp sexprs.
That is, an sexpr can be

- an atom, such as `eq`, `lambda`, or `foo`; or
- a dotted pair, such as `(a . b)`; or
- a list, such as `(this is a list)` or `(cons (quote x) (quote y))`; or
- an empty list, `()`.

Atoms must begin with an alphabetical character,
but may contains digits and the special characters "-", "_", and "?".
Atom names are case-sensitive.

Comments in the input begin with a semicolon (;).
The interpreter ignores all text from the semicolon to the end-of-line character.

The interpreter recognizes two pre-defined, global atoms

- nil
- t

Note that an empty list, `()`, is represented by the global atom `nil`,
meaning `(eq () nil)` is true.

### Data types
The dialect has only atoms and lists. Numbers and string literals are not supported.

### Functions

The interpreter implements only these primitive Lisp functions

- null
- atom
- eq
- cons
- car
- cdr

and it implements these Lisp special forms

- quote
- cond
- lambda
- label

In particular, there are no functions with side-effects.
This is a purely functional subset.

### Semantics
The dialect is dynamically scoped, in the spirit of the Manual.
It is not lexically scoped.

Note that `label` is not strictly required, because `lambda` can perform labeling
in the style of the Y cominator. But `label` is included for convenience and in the spirit of the Manual.

## Examples
These are examples of sexprs that the interpreter can evaluate.

### Hello world

This sexpr simply quotes a list.

```
(quote (hello world))

; Result: (hello world)
```

### Find first atom
This sexpr defines a function via `lambda`, labels the function as `ff`,
then applies the function to the sexpr `(quote ((a) b c))`.

```
((lambda (ff x) (ff x))
 (quote (lambda (x)
           (cond ((atom x) x)
                           ((quote t) (ff (car x))))))
 (quote ((a) b c)))

; Result: a
```

## Append lists
This example defines a function, `append`,
then uses it to append two lists, `(a b c)` and `(d e f)`.

```
; Append two lists
((lambda (append)

    ; Execute append
    (append (quote (a b c)) (quote (d e f))) )

    ; function: append
    (quote
        (lambda (a b)
            (cond ((eq a nil) b)
                  ((eq b nil) (cons a nil))
                  (t          (cons (car a) (append (cdr a) b))) )))
)

; Result: (a b c d e f)
```

### Reverse a list

This sexpr defines two functions, `reverse` and `accum`,
then uses `reverse` to reverse the order of the list `(a b c d e)`.

```
; Recursive function: reverse a list
((lambda (reverse accum)

  ; List to be reversed
  (reverse (quote (a b c d e))) )

  ; function: reverse
  (quote
    (lambda (x) (accum x nil)))

  ; function: accum
  (quote
    (lambda (sublist acc)
            (cond ((eq sublist nil) acc)
                  (t                (accum (cdr sublist) (cons (car sublist) acc))) )))
)

; Result: (e d c b a)
```
