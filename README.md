# Lisp

This is an interpreter for the subset of Lisp described in the Lisp 1.5 Programmer's Manual.
The program executes a simple REPL loop, reading a Lisp S-expression ("sexpr") from stdin or a file,
evaluating the sexpr, and printing the result, then repeating until end-of-file.

Usage: lisp [--trace] [file]

Options:
    --trace     Trace calls to eval
    file        Read input from file instead of stdin

When reading from stdin, the program prompts the user with the string ">>".

## Input syntax

The intepreter expects the usual, simple syntax for Lisp sexprs.
That is, an sexpr can be

- an atom, such as `eq`, `lambda`, or `foo`; or
- a dotted pair, such as `(a . b)`; or
- a list, such as `(this is a list)` or `(cons (quote x) (quote y))`; or
- an empty list, `()`.

The interpreter recognizes two pre-defined, global atoms

- nil
- t

Note that an empty list, `()`, is equivalent to nil.

## Functions

The interpreter implements these standard Lisp functions

- atom
- eq
- cons
- car
- cdr

and it implements these Lisp special forms

- quote
- cond
- lambda

## Comments

Comments in the input begin with a semicolon (;).
The interpreter ignores all text from the semicolon to the end-of-line character.

## Examples
These are examples of sexprs that the interpreter can evaluate.

### Hello world

This sexpr simply quotes a list.

```
(quote (hello world))

; Result: (hello world)

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
