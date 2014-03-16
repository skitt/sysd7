#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.text
LC0:
	.ascii "SK2 says hello!\0"
	.even
.globl _main
_main:
	link a6,#-28
	movel a2,sp@-
	jbsr ___main
	lea a6@(-26),a2
	pea 16:w
	movel a2,sp@-
	pea LC0
	jbsr _bcopy
	addqw #8,sp
	addqw #4,sp
	jbsr _serialinit
	moveb #1,a6@(-10)
	moveq #16,d1
	movel d1,a6@(-8)
	movel a2,a6@(-4)
	pea a6@(-10)
	jbsr _senddata
	jbsr _serialclose
	movel a6@(-32),a2
	unlk a6
	rts
