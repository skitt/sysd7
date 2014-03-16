#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.globl _pit
.data
	.even
_pit:
	.long 83886080
.globl _rtc
	.even
_rtc:
	.long 10493920
.text
	.even
.globl _number
_number:
	link a6,#0
	moveml #0x3800,sp@-
	movel a6@(8),d2
	moveq #100,d3
L5:
	movel d2,d0
	divsll d3,d2:d0
	movel d0,a0
	pea a0@(48)
	jbsr _mon_putc
	addqw #4,sp
	movel d3,d1
	mulsl #1717986919,d0:d1
	asrl #2,d0
	movel d3,d1
	moveq #31,d4
	asrl d4,d1
	movel d0,d3
	subl d1,d3
	tstl d3
	jgt L5
	moveml a6@(-12),#0x1c
	unlk a6
	rts
	.even
.globl _main
_main:
	link a6,#0
	movel a2,sp@-
	jbsr ___main
	pea 71:w
	lea _mon_putc,a2
	jbsr a2@
	jbsr _mon_getc
	pea 46:w
	jbsr a2@
	movel _pit,a0
	moveb #192,a0@(11)
	addqw #8,sp
	movel _pit,a0
L8:
	moveb a0@(43),d0
#APP
	nop
#NO_APP
	btst #4,d0
	jeq L8
	pea 69:w
	jbsr _mon_putc
	movel a6@(-4),a2
	unlk a6
	rts
