# A call to a const member function of the library.
	.text
_Z5firstRKd:
	call	_ZNK6galoot4core2mvIdE4sizeEv
	ret
_Z6secondv:
	ret
	movl	$0, %eax
