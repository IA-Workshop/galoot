# Calls through registers loaded with a library function's address as an
# immediate, as a build without position-independent code writes it: into
# the 32-bit half of the register that is called, and with movabsq.
	.text
_Z5firstRKd:
	movl	$_ZN6galoot4core3fooEv, %eax
	call	*%rax
	ret
_Z6secondv:
	movabsq	$_ZN6galoot4core3barEv, %rdx
	call	*%rdx
	ret
