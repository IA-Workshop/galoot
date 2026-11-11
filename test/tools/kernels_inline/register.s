# A call through a register that holds the address of a function of the
# library.
	.text
_Z5firstRKd:
	leaq	_ZN6galoot4core3fooEv(%rip), %rax
	call	*%rax
	ret
_Z6secondv:
	ret
