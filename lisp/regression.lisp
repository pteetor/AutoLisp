; #!/usr/bin/env /home/paul/Projects/ConsVM/consvm

;
; Some simple regression tests
;

nil

t

(quote foo)

(cons (quote x) (quote y))

(cons (quote x) nil)

(car (cons (quote x) (quote y)))

(cdr (cons (quote x) (quote y)))

(eq nil nil)
(eq t t)
(eq nil t)
(eq t nil)
(eq (quote foo) (quote foo))
(eq (quote foo) (quote fum))

(atom nil)
(atom t)
(atom (cons nil t))

(cond
   (t   (quote isTrue))
   (nil (quote isFalse))
   (t   (quote (not reached))) )

(cond
   (nil (quote isFalse))
   (t   (quote isTrue))
   (t   (quote (not reached))) )

; Nullary function
((lambda () (quote foo)))

; One-argument function
((lambda (x) (cons x x)) (quote foo))

; Two-argument function
((lambda (x y) (cons y x)) (quote second) (quote first))

; Recursive function
((lambda (reverse accum)

  ; List to be reversed
  (reverse (cons (quote last) (cons (quote first) nil))))

  ; reverse
  (quote
    (lambda (x) (accum x nil)))

  ; accum
  (quote
    (lambda (sublist acc)
            (cond ((eq sublist nil) acc)
                  (t                (accum (cdr sublist) (cons (car sublist) acc))) ))))
