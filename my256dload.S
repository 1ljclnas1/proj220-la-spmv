	.text
	.globl my256dload
	.type  my256dload, @function
my256dload:
	fld.d	$f0,$a1,0
	fld.d	$f1,$a2,0
	fld.d	$f2,$a3,0
	fld.d	$f3,$a4,0
	xvinsve0.d	$xr4,$xr0,0
	xvinsve0.d	$xr4,$xr1,1
	xvinsve0.d	$xr4,$xr2,2
	xvinsve0.d	$xr4,$xr3,3
	xvst	$xr4,$a0,0
	jr 	$ra
	
