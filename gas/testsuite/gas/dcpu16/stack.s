	;; Push
	set PUSH, A
	set [--SP], A

	;; Pop
	set A, POP
	set A, [SP++]

	;; Peek
	set A, PEEK
	set A, [SP]
	set PEEK, A
	set [SP], A
	
	;; Pick
	set A, PICK 0
	set A, [SP+0]
	set A, PICK 1
	set A, [SP+1]
	set A, PICK foo
	set A, [SP+foo]
	set A, PICK -3
	set A, [SP-3]
foo:	
	;; Direct
	set A, SP
	set SP, A
	