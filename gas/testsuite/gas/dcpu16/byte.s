	;; Byte is 16-bit wide on DCPU-16
	.byte	0, 1, 2, 3
	.byte	0x1234, 0x3003, 0x4004, 0x5005
	.byte	-1, -2, -3, -4
	
	;; Short should equal byte
	.short	0, 1, 2, 3
	.short	0x1234, 0x3003, 0x4004, 0x5005
	.short	-1, -2, -3, -4

	;; Ascii should be stored as 16 bit characters
	.ascii "Hello, "
	.asciz "World!"

	;; string8 can still be used to store 8-bit strings
	.string8 "Fail!"
	
