#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.text
	.even
.globl _puts
_puts:
	link a6,#0
	movel a2,sp@-
	movel a6@(8),a2
	tstb a2@
	jeq L3
L4:
	moveb a2@+,d1
	extbl d1
	movel d1,sp@-
	jbsr _mon_putc
	addqw #4,sp
	tstb a2@
	jne L4
L3:
	movel a6@(-4),a2
	unlk a6
	rts
