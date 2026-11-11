# A conditional tail jump to a function of the library, as LLVM can write it.
	.text
_Z5firstRKd:
	ret
_Z6secondv:                             # @_Z6secondv
	jne	_ZN6galoot4core9transformIdEENS0_2mvIT_EERKS3_ # TAILCALL
	movl	$0, %eax
	ret
