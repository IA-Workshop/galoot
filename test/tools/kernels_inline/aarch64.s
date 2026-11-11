# Two functions compiled for AArch64, where a call is bl and a tail call b,
# so a check that reads x86-64 would see no call at all.
	.text
_Z5firstRKd:
	bl	_ZN6galoot4core3fooEv
	ret
_Z6secondv:
	b	_ZN6galoot4core3barEv
