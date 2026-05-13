; Expected VM output (one number per println):
;   10  — then-branch when test is true
;   20  — else-branch when test is false

(println (if #t 10 20))
; (println (if #f 10 20))
