	.text
	.globl my256dfmadd
	.type my256dfmadd, @function
my256dfmadd:
	xvld    $xr0,$a0,0
        xvld    $xr1,$a1,0
	xvld	$xr2,$a2,0
        xvfmul.d $xr3,$xr0,$xr1
	xvfadd.d $xr4,$xr3,$xr2
	xvst	$xr4,$a2,0
	jr	$ra
