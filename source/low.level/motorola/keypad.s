#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.globl _pit
.data
	.even
_pit:
	.long 83886080
.text
	.even
.globl _main
_main:
	link a6,#-4
	moveml #0x3020,sp@-
	jbsr ___main
	clrb a6@(-1)
	movel _pit,a0
	moveb #192,a0@(11)
	clrl d3
	clrl d2
	lea _mon_putc,a2
L5:
	movel _pit,a0
	jra L18
L8:
	moveb a6@(-1),d0
	eorb #1,d0
	moveb d0,a6@(-1)
L18:
	moveb a0@(43),d0
	btst #4,d0
	jeq L8
	movel _pit,a0
	jra L19
L12:
	moveb a6@(-1),d0
	eorb #1,d0
	moveb d0,a6@(-1)
L19:
	moveb a0@(43),d0
	btst #4,d0
	jne L12
	movel _pit,a0
	moveb a0@(43),d0
	andb #15,d0
	moveb d0,a6@(-1)
	moveb a6@(-1),d0
	cmpb #10,d0
	jls L14
	moveb a6@(-1),d0
	moveb d0,d2
	movel d2,a1
	pea a1@(55)
	jra L20
L14:
	moveb a6@(-1),d0
	andl #0xFF,d0
	movel d0,a1
	pea a1@(48)
L20:
	jbsr _mon_putc
	movel #10,sp@
	jbsr a2@
	pea 13:w
	jbsr a2@
	addqw #8,sp
	addql #1,d3
	moveq #19,d1
	cmpl d3,d1
	jge L5
	moveml a6@(-16),#0x40c
	unlk a6
	rts
