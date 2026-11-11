# A jump through a register with the notrack prefix of indirect-branch
# tracking.
	.text
_Z5firstRKd:
	leaq	_ZN6galoot4core3fooEv(%rip), %rax
	notrack jmp	*%rax
_Z6secondv:
	movq	%rdi, %rax
	ret
