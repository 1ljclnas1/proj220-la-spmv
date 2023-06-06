	.text
	.globl	my256dadd
	.type my256dadd, @function
my256dadd:
	xvld	$xr0,$a0,0
	xvld	$xr1,$a1,0
.ADD:
	xvfadd.d	$xr2,$xr0,$xr1
.STORE:
	xvst	$xr2,$a2,0
.END:
	jr      $ra
