;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                  Language Technologies Institute                    ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                         Copyright (c) 2002                          ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;             Author: Alan W Black (awb@cs.cmu.edu)                   ;;;
;;;               Date: August 2002                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;; Generate a C compilable lts rewrite rules.                          ;;;
;;;                                                                     ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;      Modified by:  Fabio Tesser <tesser@pd.istc.cnr.it>             ;;; 
;;;             Date:  October 2004                                     ;;; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require 'make_vallist)

(define (lts_norm_rule rule)
  (let (q w (q2pass nil))

    (mapcar
     (lambda (l)
       (cond
	((string-equal l "[")
	  (set! q  (list w))
	  (set! w nil)
	  ;(format t "q1=%l\n" q) 
	  )
	((string-equal l "]")
	  (set! q (cons (reverse w) q))
	  (set! w nil)
	  (set! q2pass t)
	  ;(format t "q2=%l\n" q) 
	  )
	((string-equal l "=")
	  (set! q (cons (reverse w) q))
	  (set! w nil)
	  (set! q2pass nil)
	  ;(format t "q3=%l\n" q) 
	  )
	(t
	 (if (and q2pass (or (string-equal l "*") (string-equal l "+"))) ;e se passato q2
	     (set! w (cons (car w)(cons l (cdr w))))
	     (set! w (cons l w)))
	 ;(format t "w_=%l\n" w) 
	 )))
     rule)

    (set! xxx (reverse (cons (reverse w) q)))
    (format t "%l %l\n" rule xxx)
    xxx))

(define (ltsrewritestoC fname odir)
  "(ltsrewritestoC fname odir) modify by fabio"

  (let 
    ((all_rules (load fname t)))
    (while all_rules
	   (if (string-equal (car(car all_rules)) "lts.ruleset")
	       (let ((rules (car all_rules)) name ofde ofdh)
		 (set! name  (car (cdr rules)))
		 (print name)
		 (set! ofde (fopen (path-append odir (string-append name "_lts_rewrites.c")) "w"))
		 (set! ofdh (fopen (path-append odir (string-append name "_lts_rewrites.h")) "w"))
		 (format ofde "/*******************************************************/\n")
		 (format ofde "/**  Autogenerated lts rewrite rules for %s     */\n" name)
		 (format ofde "/**  from %s    */\n" name)
		 (format ofde "/*******************************************************/\n")
		 (format ofde "\n")
		 (format ofde "#include \"cst_string.h\"\n")
		 (format ofde "#include \"cst_val.h\"\n")
		 (format ofde "#include \"cst_lts_rewrites.h\"\n")
		 (format ofdh "extern const cst_lts_rewrites %s_lts_rewrites;\n\n" name)
		 ;;insieme (VNOACCENT a e i o u E O)
		 (format t "insieme: %l\n" (car (cdr (cdr rules))))
		 (cellstovals 
		  (format nil "%s_lts_sets" name)
		  (car (cdr (cdr rules)))
		  ofde)
		 (set! eoc_sets cells_count)
		 (cellstovals 
		  (format nil "%s_lts_rules" name)
		  (mapcar
		   lts_norm_rule
		   (car (cdr (cdr (cdr rules)))))
		  ofde)
		 
		 (format ofde "#define %s_lts_sets &%s_lts_sets_%04d\n" 
			 name name eoc_sets)
		 (format ofde "#define %s_lts_rules &%s_lts_rules_%04d\n" 
			 name name cells_count)
		 
		 (format ofde "\n")
		 (format ofde "const cst_lts_rewrites %s_lts_rewrites  = {\n" name)
		 (format ofde "   \"%s\",\n" name)
		 (format ofde "   %s_lts_sets,\n" name)
		 (format ofde "   %s_lts_rules,\n" name)
		 (format ofde "};\n")
		 (format ofde "\n")
		 (fclose ofde)
		 (fclose ofdh)))
	   (set! all_rules (cdr all_rules)))))




(provide 'make_lts_rewrite)
