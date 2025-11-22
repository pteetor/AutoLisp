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
