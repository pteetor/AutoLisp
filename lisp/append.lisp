; Append two lists
((lambda (append)
    (append (quote (a b c)) (quote (d e f))) )

    ; function: append
    (quote
        (lambda (a b)
            (cond ((eq a nil) b)
                  ((eq b nil) (cons a nil))
                  (t          (cons (car a) (append (cdr a) b))) )))
)
