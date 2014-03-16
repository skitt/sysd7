#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.globl _uartw
.data
	.even
_uartw:
	.long 11534336
.globl _uartr
	.even
_uartr:
	.long 11534336
.globl _pit
	.even
_pit:
	.long 12582912
.globl _irqvect
	.even
_irqvect:
	.long 0
.text
LC0:
	.ascii "Interrupt!\12\15\0"
	.even
.globl _Cserialirq
_Cserialirq:
	link a6,#0
	pea LC0
	jbsr _puts
	unlk a6
	rts
LC1:
	.ascii "Initialising PIT...\0"
LC2:
	.ascii " done.\12\15\0"
LC3:
	.ascii "Initialising serial channel B...\0"
LC4:
	.ascii "Enabling interrupts...\0"
	.even
.globl _serialinit
_serialinit:
	link a6,#0
	movel a2,sp@-
	pea LC1
	lea _puts,a2
	jbsr a2@
	movel _irqvect,a0
	movel #_serialirq,a0@(264)
	movel _pit,a0
	clrb a0@(3)
	movel _pit,a0
	moveb #24,a0@(7)
	movel _pit,a0
	clrb a0@(15)
	movel _pit,a0
	moveb #130,a0@(31)
	movel _pit,a0
	moveb #64,a0@(23)
	movel _pit,a0
	moveb #32,a0@(3)
	pea LC2
	jbsr a2@
	pea LC3
	jbsr a2@
	movel _uartw,a0
	moveb #16,a0@(43)
	movel _uartw,a0
	moveb #3,a0@(35)
	movel _uartw,a0
	moveb #7,a0@(35)
	movel _uartw,a0
	moveb #204,a0@(39)
	movel _irqvect,a0
	movel #_serialirq,a0@(1020)
	movel _uartw,a0
	moveb #15,a0@(51)
	movel _uartw,a0
	moveb #48,a0@(23)
	movel _uartw,a0
	moveb #32,a0@(43)
	movel _uartw,a0
	moveb #48,a0@(43)
	movel _uartw,a0
	moveb #64,a0@(43)
	movel _uartw,a0
	moveb #5,a0@(43)
	pea LC2
	jbsr a2@
	pea LC4
	jbsr a2@
#APP
	move.w 0x2000, sr
#NO_APP
	pea LC2
	jbsr a2@
	movel a6@(-4),a2
	unlk a6
	rts
	.even
.globl _serialtransmit
_serialtransmit:
	link a6,#0
	moveb a6@(11),d1
	movel _uartr,a0
L4:
	moveb a0@(39),d0
	btst #2,d0
	jeq L4
	movel _uartw,a0
	moveb d1,a0@(47)
	unlk a6
	rts
	.even
.globl _serialreceive
_serialreceive:
	link a6,#0
	movel _uartr,a0
L9:
	moveb a0@(39),d0
	btst #0,d0
	jeq L9
	movel _uartr,a0
	moveb a0@(47),d0
	andl #0xFF,d0
	unlk a6
	rts
LC5:
	.ascii "Waiting for a character... \0"
LC6:
	.ascii "\12\15\0"
LC7:
	.ascii "Initialising camera...\0"
LC8:
	.ascii " done.\12\15Grabbing frame...\0"
LC9:
	.ascii "Transmitting frame...\0"
	.even
.globl _main
_main:
	link a6,#0
	moveml #0x2030,sp@-
	jbsr ___main
	jbsr _serialinit
	pea LC5
	lea _puts,a2
	jbsr a2@
	jbsr _serialreceive
	clrl sp@-
	moveb d0,sp@(3)
	jbsr _mon_putc
	pea LC6
	jbsr a2@
	pea LC7
	jbsr a2@
	jbsr _QC_init
	pea LC8
	jbsr a2@
	pea _buffer
	jbsr _QC_grabframe
	pea LC2
	jbsr a2@
	pea LC9
	jbsr a2@
	subl a2,a2
	addw #32,sp
	lea _buffer,a3
	clrl d2
L17:
	moveb a3@(a2:l),d0
	addb #48,d0
	moveb d0,d2
	movel d2,sp@-
	jbsr _serialtransmit
	addqw #4,sp
	addqw #1,a2
	cmpl #19199,a2
	jle L17
	pea LC2
	jbsr _puts
	moveml a6@(-12),#0xc04
	unlk a6
	rts
.comm _buffer,19200
.comm _queue,65536
