#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.globl _pin_pos
.data
	.even
_pin_pos:
	.long 0
.globl _got_uid
	.even
_got_uid:
	.long 0
.text
LC0:
	.ascii "Waiting for connection...\0"
LC1:
	.ascii "sent ping\12\15\0"
LC2:
	.ascii "connected\12\15\0"
LC3:
	.ascii "Closing serial\12\15\0"
LC4:
	.ascii "Converted string: \0"
LC5:
	.ascii "Enter PIN: \0"
	.even
.globl _main
_main:
	link a6,#-8
	moveml #0x3030,sp@-
	jbsr ___main
	jbsr _boardinit
	jbsr _serialinit
	pea LC0
	lea _puts,a2
	jbsr a2@
	pea 255:w
	jbsr _serialtransmit
	pea LC1
	jbsr a2@
	jbsr _serialreceive
	addqw #8,sp
	addqw #4,sp
	cmpb #255,d0
	jne L2
	pea LC2
	jbsr a2@
	jbsr _request_db
	pea LC3
	jbsr a2@
	addqw #8,sp
L2:
	jbsr _serialclose
L3:
	jbsr _checkring
	tstl d0
	jeq L3
	jbsr _getringinfo
	movel d0,a0
	moveb a0@+,a6@(-8)
	moveb a0@+,a6@(-7)
	moveb a0@+,a6@(-6)
	moveb a0@+,a6@(-5)
	moveb a0@+,a6@(-4)
	moveb a0@+,a6@(-3)
	moveb a0@+,a6@(-2)
	moveb a0@,a6@(-1)
	movel #10000000,sp@-
	jbsr _starttimer
	pea 1:w
	jbsr _buzzerled
	addqw #8,sp
L6:
	jbsr _gettimer
	cmpl #9970000,d0
	jgt L6
	clrl sp@-
	jbsr _buzzerled
	addqw #4,sp
L9:
	jbsr _gettimer
	cmpl #9960000,d0
	jgt L9
	pea 1:w
	jbsr _buzzerled
	addqw #4,sp
L12:
	jbsr _gettimer
	cmpl #9930000,d0
	jgt L12
	clrl sp@-
	jbsr _buzzerled
	jbsr _stoptimer
	subl a2,a2
	addqw #4,sp
	clrl d2
	lea _uid_no,a3
L18:
	movel a3,sp@-
	moveb a2@(-7,a6:l),d2
	movel d2,sp@-
	jbsr _conv_hchar
	addqw #8,sp
	addqw #2,a3
	addqw #1,a2
	moveq #5,d1
	cmpl a2,d1
	jge L18
	lea _uid_no+12,a3
	clrb a3@
	pea LC4
	lea _puts,a2
	jbsr a2@
	pea a3@(-12)
	jbsr a2@
	moveq #1,d1
	movel d1,_got_uid
	clrl _pin_pos
	pea LC5
	jbsr a2@
	subl a2,a2
	addqw #8,sp
	addqw #4,sp
	clrl d3
L24:
	jbsr _checkkeypad
	tstl d0
	jeq L24
	jbsr _getkeypad
	moveb d0,d2
	movel #10000000,sp@-
	jbsr _starttimer
	pea 1:w
	jbsr _buzzerled
	addqw #8,sp
L27:
	jbsr _gettimer
	cmpl #9970000,d0
	jgt L27
	clrl sp@-
	jbsr _buzzerled
	moveb d2,d3
	movel d3,sp@-
	jbsr _send_keypad_digit
	addqw #8,sp
L30:
	jbsr _checkkeypad
	tstl d0
	jne L30
	addqw #1,a2
	moveq #3,d1
	cmpl a2,d1
	jge L24
	moveml a6@(-24),#0xc0c
	unlk a6
	rts
LC6:
	.ascii "The door is unlocked\0"
	.even
.globl _refuse_entry
_refuse_entry:
	link a6,#0
	movel d2,sp@-
	clrl d2
L38:
	pea 3:w
	jbsr _buzzerled
	movel #210000,sp@-
	jbsr _starttimer
	addqw #8,sp
L39:
	jbsr _gettimer
	cmpl #110000,d0
	jgt L39
	clrl sp@-
	jbsr _buzzerled
	addqw #4,sp
L42:
	jbsr _gettimer
	cmpl #10000,d0
	jgt L42
	jbsr _stoptimer
	addql #1,d2
	moveq #4,d1
	cmpl d2,d1
	jge L38
	pea LC6
	jbsr _puts
	movel a6@(-4),d2
	unlk a6
	rts
	.even
.globl _unlock_door
_unlock_door:
	link a6,#0
	movel #3010000,sp@-
	jbsr _starttimer
	pea 2:w
	jbsr _buzzerled
	addqw #8,sp
L47:
	jbsr _gettimer
	cmpl #2010000,d0
	jgt L47
L50:
	jbsr _gettimer
	cmpl #10000,d0
	jle L51
	jbsr _getdoorstatus
	tstl d0
	jeq L50
L51:
	jbsr _stoptimer
	clrl sp@-
	jbsr _buzzerled
	unlk a6
	rts
.comm _uid_no,14
.comm _pin,6
.comm _db_local_store,180
