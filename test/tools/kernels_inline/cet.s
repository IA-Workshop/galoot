# Two functions whose results hold no blade, built with -fcf-protection:
# each starts with endbr64, and the file ends with the .note.gnu.property
# section, whose numeric labels are not functions.
	.text
	.globl	_Z5firstRKd
	.type	_Z5firstRKd, @function
_Z5firstRKd:
.LFB0:
	.cfi_startproc
	endbr64
	ret
	.cfi_endproc
.LFE0:
	.globl	_Z6secondv
	.type	_Z6secondv, @function
_Z6secondv:
	endbr64
	ret
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
