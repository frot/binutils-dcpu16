	;; Test [reg+offset]
	set	A, [0x12]
	set	A, [A+0x34]
	
	set	A, [label]
	set	A, [A+label]
	
	set	A, [ext_label]
	set	A, [A+ext_label]
label:
