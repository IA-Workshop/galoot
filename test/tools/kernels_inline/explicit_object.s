# A call to an explicit-object member function of the library, whose name
# is mangled with H after the N.
	.text
_Z5firstRKd:
	call	_ZNH6galoot4core3fooIdEEvRKNS0_2mvIdEE
	movl	$0, %eax
	ret
_Z6secondv:
	ret
