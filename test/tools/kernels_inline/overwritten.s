# A register loaded with a library address and then overwritten before a
# call through it, which then calls something else.
	.text
_Z5firstRKd:
	leaq	_ZN6galoot4core3fooEv(%rip), %rax
	movq	%rdi, %rax
	call	*%rax
	ret
_Z6secondv:
	ret
