# Two functions that call no function of the library: local jumps, a call
# of the standard library through the PLT, a call and a tail jump to symbols
# whose template arguments name galoot types, a namespace named galoot
# inside another namespace, the address of the library's data loaded with
# no call through it, and a call through a register that holds another
# symbol.
	.text
	.globl	_Z5firstRKd
	.type	_Z5firstRKd, @function
_Z5firstRKd:
.LFB0:
	testq	%rdi, %rdi
	je	.L2
	jmp	.L3
.L2:
	call	memcpy@PLT
	callq	_ZNSt5arrayIN6galoot4core14oriented_bladeELm4EE4fillERKS2_
.L3:
	call	_ZN4test6galoot4stepEv
	ret
.LFE0:
	.globl	_Z6secondv
	.type	_Z6secondv, @function
_Z6secondv:                             # @_Z6secondv
# %bb.0:
	leaq	_ZN6galoot4core6detail5tableE(%rip), %rdx
	movsd	(%rdx), %xmm0
	leaq	_ZNSt6vectorIiE5clearEv(%rip), %rax
	call	*%rax
	jmp	_ZNSt6vectorIN6galoot4core2mvEE5clearEv # TAILCALL
.Lfunc_end0:
