((label append (lambda (x y)
  (cond ((null x) y)
        (t (cons (car x) (append (cdr x) y))))))
 (quote (a b)) (quote (c d)))
