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
.globl _bpit
	.even
_bpit:
	.long 12582912
.globl _irqvect
	.even
_irqvect:
	.long 0
.text
	.even
.globl _defaulthandler
_defaulthandler:
	link a6,#0
	unlk a6
	rts
	.even
.globl _serialevent
_serialevent:
	link a6,#0
	moveml #0x3820,sp@-
	tstl _serialactive
	jeq L2
	movel _uartr,a0
	moveb a0@(39),d1
	movel _uartw,a0
	clrb a0@(23)
	btst #2,d1
	jeq L4
	movel _serialoutlen,d0
	jeq L4
	movel _uartw,a1
	movel _serialoutreadidx,d0
	lea _serialout,a0
	moveb a0@(d0:l),a1@(47)
	movel _serialoutreadidx,d0
	addql #1,d0
	andl #65535,d0
	movel d0,_serialoutreadidx
	movel _serialoutlen,d0
	subql #1,d0
	movel d0,_serialoutlen
	movel _serialoutlen,d0
L4:
	btst #0,d1
	jeq L6
	lea _serialin,a1
L9:
	movel _serialinidx,d0
	cmpl #32767,d0
	jhi L6
	movel _serialinidx,d0
	movel _uartr,a0
	moveb a0@(47),a1@(d0:l)
	movel _serialinidx,d0
	addql #1,d0
	movel d0,_serialinidx
	movel _serialinidx,d0
	movel _uartr,a0
	moveb a0@(47),d1
	btst #0,d1
	jne L9
L6:
	tstl _serialinstate
	jne L10
	movel _serialinidx,d0
	moveq #4,d4
	cmpl d0,d4
	jcc L10
	lea _pkt,a0
	lea _serialin,a1
	moveb a1@,a0@
	lea _pkt+2,a0
	clrl a0@
	moveq #1,d2
	clrl d3
L14:
	movel a0@,d1
	asll #8,d1
	moveb a1@(d2:l),d0
	moveb d0,d3
	addl d3,d1
	movel d1,a0@
	addql #1,d2
	moveq #4,d4
	cmpl d2,d4
	jge L14
	moveq #5,d2
	movel _serialinidx,d0
	cmpl d2,d0
	jls L17
	lea _serialin-5,a0
	lea a0@(5),a1
L19:
	moveb a1@(d2:l),a0@(d2:l)
	addql #1,d2
	movel _serialinidx,d0
	cmpl d2,d0
	jhi L19
L17:
	movel _serialinidx,d0
	subql #5,d0
	movel d0,_serialinidx
	moveq #1,d4
	movel d4,_serialinstate
L10:
	moveq #1,d4
	cmpl _serialinstate,d4
	jne L21
	lea _pkt+2,a0
	movel _serialinidx,d0
	movel a0@,d1
	cmpl d0,d1
	jhi L21
	clrl d2
	cmpl d2,d1
	jls L23
	lea _pktbuf,a2
	lea _serialin,a1
L25:
	moveb a1@(d2:l),a2@(d2:l)
	addql #1,d2
	cmpl a0@,d2
	jcs L25
L23:
	lea _pkt+2,a0
	movel a0@,d2
	movel _serialinidx,d0
	cmpl d2,d0
	jls L28
	movel a0,a1
	lea _serialin,a0
L30:
	movel d2,d0
	subl a1@,d0
	moveb a0@(d2:l),a0@(d0:l)
	addql #1,d2
	movel _serialinidx,d0
	cmpl d2,d0
	jhi L30
L28:
	lea _pkt+2,a0
	movel _serialinidx,d0
	subl a0@,d0
	movel d0,_serialinidx
	movel #_pktbuf,_pkt+6
	lea a0@(-2),a1
	clrl d0
	moveb a1@,d0
	lea _handlers,a0
	movel a0@(d0:l:4),a0
	tstl a0
	jeq L32
	movel a1,sp@-
	jbsr a0@
L32:
	clrl _serialinstate
L21:
	movel _serialoutlen,d0
	jne L33
	movel _uartw,a0
	moveb #32,a0@(23)
	jra L2
L33:
	movel _uartw,a0
	moveb #48,a0@(23)
L2:
	moveml a6@(-16),#0x41c
	unlk a6
	rts
	.even
.globl _pitinit
_pitinit:
	link a6,#0
	movel _irqvect,a0
	movel #_serialirq,a0@(264)
	movel _bpit,a0
	clrb a0@(3)
	movel _bpit,a0
	moveb #24,a0@(7)
	movel _bpit,a0
	moveb #130,a0@(31)
	movel _bpit,a0
	moveb #64,a0@(23)
	movel _bpit,a0
	moveb #48,a0@(3)
	unlk a6
	rts
	.even
.globl _serialinit
_serialinit:
	link a6,#0
	clrl _serialoutlen
	movel _serialoutlen,d0
	movel d0,_serialoutreadidx
	movel _serialoutreadidx,d0
	movel d0,_serialoutwriteidx
	movel _serialoutwriteidx,d0
	movel d0,_serialinidx
	clrl _serialinstate
	jbsr _pitinit
	movel _uartw,a0
	moveb #16,a0@(43)
	movel _uartw,a0
	moveb #131,a0@(35)
	movel _uartw,a0
	moveb #7,a0@(35)
	movel _uartw,a0
	moveb #204,a0@(39)
	movel _uartw,a0
	moveb #48,a0@(23)
	moveq #1,d1
	movel d1,_serialactive
	movel _uartw,a0
	moveb #32,a0@(43)
	movel _uartw,a0
	moveb #48,a0@(43)
	movel _uartw,a0
	moveb #64,a0@(43)
	movel _uartw,a0
	moveb #5,a0@(43)
#APP
	andi #0xF8FF, sr
#NO_APP
	clrl d0
	unlk a6
	rts
	.even
.globl _serialclose
_serialclose:
	link a6,#0
L38:
	movel _serialinidx,d0
	jne L38
	movel _serialoutlen,d0
	jne L38
	movel _uartw,a0
	clrb a0@(23)
	clrl _serialactive
	unlk a6
	rts
	.even
.globl _senddata
_senddata:
	link a6,#0
	moveml #0x3020,sp@-
	movel a6@(8),a1
L43:
	movel _serialoutlen,d0
	cmpl #65536,d0
	jeq L43
	movel _serialoutwriteidx,d0
	lea _serialout,a0
	moveb a1@,a0@(d0:l)
	movel _serialoutwriteidx,d0
	addql #1,d0
	andl #65535,d0
	movel d0,_serialoutwriteidx
	movel _serialoutlen,d0
	addql #1,d0
	movel d0,_serialoutlen
	movel _serialoutlen,d0
	moveq #24,d2
	moveq #0,d3
	notb d3
L50:
L51:
	movel _serialoutlen,d0
	cmpl #65536,d0
	jeq L51
	movel _serialoutwriteidx,d1
	movel d3,d0
	asll d2,d0
	andl a1@(2),d0
	lsrl d2,d0
	moveb d0,a0@(d1:l)
	movel _serialoutwriteidx,d0
	addql #1,d0
	andl #65535,d0
	movel d0,_serialoutwriteidx
	movel _serialoutlen,d0
	addql #1,d0
	movel d0,_serialoutlen
	movel _serialoutlen,d0
	subql #8,d2
	jpl L50
	clrl d2
	cmpl a1@(2),d2
	jcc L57
	lea _serialout,a2
L59:
L60:
	movel _serialoutlen,d0
	cmpl #65536,d0
	jeq L60
	movel _serialoutwriteidx,d0
	movel a1@(6),a0
	moveb a0@(d2:l),a2@(d0:l)
	movel _serialoutwriteidx,d0
	addql #1,d0
	andl #65535,d0
	movel d0,_serialoutwriteidx
	movel _serialoutlen,d0
	addql #1,d0
	movel d0,_serialoutlen
	movel _serialoutlen,d0
	addql #1,d2
	cmpl a1@(2),d2
	jcs L59
L57:
	jbsr _serialevent
	clrl d0
	moveml a6@(-12),#0x40c
	unlk a6
	rts
	.even
.globl _registerhandler
_registerhandler:
	link a6,#0
	clrl d0
	moveb a6@(11),d0
	lea _handlers,a0
	movel a6@(12),a0@(d0:l:4)
	clrl d0
	unlk a6
	rts
	.even
.globl _unregisterhandler
_unregisterhandler:
	link a6,#0
	moveb a6@(11),d0
	cmpb #1,d0
	jls L67
	andl #0xFF,d0
	lea _handlers,a0
	clrl a0@(d0:l:4)
	jra L68
L67:
	andl #0xFF,d0
	lea _handlers,a0
	movel #_defaulthandler,a0@(d0:l:4)
L68:
	unlk a6
	rts
	.even
.globl _grabframe
_grabframe:
	link a6,#0
	movel d2,sp@-
	jbsr _serialclose
	jbsr _QC_init
	movel d0,d2
	jne L70
	movel a6@(8),sp@-
	jbsr _QC_grabframe
	movel d0,d2
	addqw #4,sp
L70:
	jbsr _serialinit
	movel d2,d0
	movel a6@(-4),d2
	unlk a6
	rts
.comm _serialin,32768
.comm _serialinidx,4
.comm _serialout,65536
.comm _serialoutwriteidx,4
.comm _serialoutreadidx,4
.comm _serialoutlen,4
.comm _pkt,10
.comm _pktbuf,32768
.comm _handlers,1024
.comm _serialinstate,4
.comm _serialactive,4
