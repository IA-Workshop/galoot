# Calls to a function of the library through the PLT, and through the
# global offset table, as a compile with -fno-plt writes it.
	.text
_Z5firstRKd:
	call	_ZN6galoot4core5applyIdEEvv@PLT
	ret
_Z6secondv:
	call	*_ZN6galoot4core3sumIdEEvv@GOTPCREL(%rip)
	ret
	movl	$0, %eax
