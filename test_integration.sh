#!/bin/bash

LISP=./autolisp

echo "Running Integration Tests..."

# Trivial
echo "Test: Trivial"
$LISP lisp_examples/trivial.lisp > out.txt
diff out.txt <(echo -e "(a . b)\na\n(b)\nt\nnil")
if [ $? -eq 0 ]; then
    echo "PASS"
else
    echo "FAIL"
    cat out.txt
    exit 1
fi

# Append
echo "Test: Append"
$LISP lisp_examples/append.lisp > out.txt
diff out.txt <(echo "(a b c d)")
if [ $? -eq 0 ]; then
    echo "PASS"
else
    echo "FAIL"
    cat out.txt
    exit 1
fi

# Reverse
echo "Test: Reverse"
$LISP lisp_examples/reverse.lisp > out.txt
diff out.txt <(echo "(d c b a)")
if [ $? -eq 0 ]; then
    echo "PASS"
else
    echo "FAIL"
    cat out.txt
    exit 1
fi

echo "All Integration Tests Passed."
rm out.txt
