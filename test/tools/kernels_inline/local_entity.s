# A call to a lambda local to a function of the library.
	.text
_Z5firstRKd:
	call	_ZZN6galoot4core3fooEvENKUlvE_clEv
	ret
_Z6secondv:
	ret
	movl	$0, %eax
