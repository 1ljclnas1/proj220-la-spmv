	.text
	.globl my256dmul
	.type my256dmul, @function
my256dmul:
	xvld 	$xr0,$a0,0
	xvld 	$xr1,$a1,0
.MUL:
	XVFMUL.D $xr2,$xr0,$xr1
.STORE:
	XVST $xr2,$a2,0
.END:
	jr	$ra
