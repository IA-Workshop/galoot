	.file	"asm.cpp"
	.text
	.p2align 4
	.globl	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.type	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, @function
_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_:
.LFB1815:
	.cfi_startproc
	movsd	32(%rdi), %xmm0
	movsd	40(%rsi), %xmm1
	mulsd	32(%rsi), %xmm0
	mulsd	40(%rdi), %xmm1
	addsd	%xmm0, %xmm1
	movsd	48(%rsi), %xmm0
	mulsd	48(%rdi), %xmm0
	addsd	%xmm1, %xmm0
	movsd	56(%rdi), %xmm1
	mulsd	56(%rsi), %xmm1
	addsd	%xmm1, %xmm0
	ret
	.cfi_endproc
.LFE1815:
	.size	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, .-_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.p2align 4
	.globl	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.type	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, @function
_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_:
.LFB1913:
	.cfi_startproc
	movsd	24(%rsi), %xmm1
	movsd	16(%rdi), %xmm0
	mulsd	24(%rdi), %xmm1
	mulsd	16(%rsi), %xmm0
	addsd	%xmm1, %xmm0
	movsd	8(%rsi), %xmm1
	mulsd	8(%rdi), %xmm1
	addsd	%xmm0, %xmm1
	movsd	(%rdi), %xmm0
	mulsd	(%rsi), %xmm0
	addsd	%xmm1, %xmm0
	ret
	.cfi_endproc
.LFE1913:
	.size	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, .-_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.p2align 4
	.globl	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE
	.type	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE, @function
_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE:
.LFB1985:
	.cfi_startproc
	movsd	32(%rsi), %xmm0
	movsd	40(%rsi), %xmm2
	movq	%rdi, %rax
	xorpd	.LC0(%rip), %xmm0
	movsd	24(%rsi), %xmm1
	unpcklpd	%xmm0, %xmm2
	movsd	%xmm1, 16(%rdi)
	movups	%xmm2, (%rdi)
	ret
	.cfi_endproc
.LFE1985:
	.size	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE, .-_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE
	.p2align 4
	.globl	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.type	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE, @function
_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE:
.LFB2055:
	.cfi_startproc
	movsd	24(%rdi), %xmm0
	xorpd	.LC0(%rip), %xmm0
	ret
	.cfi_endproc
.LFE2055:
	.size	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE, .-_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.p2align 4
	.globl	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
	.type	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE, @function
_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE:
.LFB2113:
	.cfi_startproc
	movsd	.LC2(%rip), %xmm3
	movupd	(%rdx), %xmm4
	movq	%rdi, %rax
	movupd	(%rsi), %xmm9
	movsd	(%rsi), %xmm5
	unpcklpd	%xmm3, %xmm3
	movupd	16(%rdx), %xmm2
	movapd	%xmm4, %xmm6
	movapd	%xmm4, %xmm8
	xorpd	%xmm4, %xmm3
	movapd	%xmm9, %xmm11
	unpckhpd	%xmm6, %xmm6
	movapd	%xmm3, %xmm1
	unpckhpd	%xmm11, %xmm11
	movapd	%xmm2, %xmm0
	unpckhpd	%xmm1, %xmm1
	xorpd	.LC0(%rip), %xmm0
	movapd	%xmm6, %xmm7
	mulsd	%xmm1, %xmm5
	movapd	%xmm3, %xmm1
	movapd	%xmm2, %xmm6
	mulsd	%xmm11, %xmm1
	unpcklpd	%xmm0, %xmm0
	mulpd	%xmm9, %xmm0
	subsd	%xmm1, %xmm5
	movupd	16(%rsi), %xmm1
	movapd	%xmm1, %xmm9
	unpcklpd	%xmm1, %xmm9
	mulpd	%xmm9, %xmm3
	subpd	%xmm3, %xmm0
	movapd	%xmm1, %xmm3
	unpckhpd	%xmm1, %xmm3
	unpckhpd	%xmm1, %xmm1
	mulpd	%xmm4, %xmm3
	movapd	%xmm7, %xmm4
	mulsd	%xmm5, %xmm4
	mulsd	%xmm8, %xmm5
	xorpd	.LC0(%rip), %xmm4
	mulsd	%xmm2, %xmm1
	unpcklpd	%xmm5, %xmm4
	movapd	%xmm2, %xmm5
	unpcklpd	%xmm2, %xmm5
	mulpd	%xmm0, %xmm5
	subpd	%xmm5, %xmm4
	movapd	%xmm2, %xmm5
	unpckhpd	%xmm2, %xmm5
	unpckhpd	%xmm2, %xmm2
	mulpd	%xmm3, %xmm5
	mulsd	%xmm1, %xmm2
	mulsd	%xmm6, %xmm1
	subpd	%xmm5, %xmm4
	movups	%xmm4, (%rdi)
	movapd	%xmm0, %xmm4
	unpckhpd	%xmm0, %xmm0
	mulsd	%xmm7, %xmm0
	mulsd	%xmm8, %xmm4
	addsd	%xmm0, %xmm4
	movapd	%xmm3, %xmm0
	unpckhpd	%xmm3, %xmm3
	mulsd	%xmm8, %xmm0
	mulsd	%xmm7, %xmm3
	subsd	%xmm2, %xmm4
	movsd	%xmm4, 16(%rdi)
	addsd	%xmm0, %xmm3
	addsd	%xmm1, %xmm3
	movsd	%xmm3, 24(%rdi)
	ret
	.cfi_endproc
.LFE2113:
	.size	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE, .-_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC0:
	.long	0
	.long	-2147483648
	.long	0
	.long	0
	.set	.LC2,.LC0
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
