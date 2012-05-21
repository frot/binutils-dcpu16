	;; Normal instructions
	.byte 0x00
	SET A, A
	ADD A, A
	SUB A, A
	MUL A, A
	MLI A, A
	DIV A, A
	DVI A, A
	MOD A, A
	MDI A, A
	AND A, A
	BOR A, A
	XOR A, A
	SHR A, A
	ASR A, A
	SHL A, A
	IFB A, A
	IFC A, A
	IFE A, A
	IFN A, A
	IFG A, A
	IFA A, A
	IFL A, A
	IFU A, A
	.byte 0x18
	.byte 0x19
	ADX A, A
	SBX A, A
	.byte 0x1c
	.byte 0x1d
	STI A, A
	STD A, A

	;; Extended instructions
	JSR A
	INT A
	IAG A
	IAS A
	RFI A
	IAQ A
	HWN A
	HWQ A
	HWI A
