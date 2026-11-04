	.att_syntax
	.file	"asm.cpp"
	.text
	.globl	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_ # -- Begin function _Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.prefalign	4, .Lfunc_end0, nop
	.type	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_,@function
_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_: # @_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.cfi_startproc
# %bb.0:
	movupd	32(%rsi), %xmm0
	movupd	48(%rsi), %xmm1
	movupd	32(%rdi), %xmm2
	mulpd	%xmm0, %xmm2
	movupd	48(%rdi), %xmm0
	mulpd	%xmm1, %xmm0
	movapd	%xmm2, %xmm1
	unpckhpd	%xmm2, %xmm1                    # xmm1 = xmm1[1],xmm2[1]
	addsd	%xmm2, %xmm1
	addsd	%xmm0, %xmm1
	unpckhpd	%xmm0, %xmm0                    # xmm0 = xmm0[1,1]
	addsd	%xmm1, %xmm0
	retq
.Lfunc_end0:
	.size	_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, .Lfunc_end0-_Z10dot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.cfi_endproc
                                        # -- End function
	.globl	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_ # -- Begin function _Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.prefalign	4, .Lfunc_end1, nop
	.type	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_,@function
_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_: # @_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.cfi_startproc
# %bb.0:
	movupd	(%rdi), %xmm0
	movupd	16(%rdi), %xmm1
	movupd	(%rsi), %xmm2
	mulpd	%xmm0, %xmm2
	movupd	16(%rsi), %xmm0
	mulpd	%xmm1, %xmm0
	movapd	%xmm0, %xmm1
	unpckhpd	%xmm0, %xmm1                    # xmm1 = xmm1[1],xmm0[1]
	addsd	%xmm0, %xmm1
	movapd	%xmm2, %xmm0
	unpckhpd	%xmm2, %xmm0                    # xmm0 = xmm0[1],xmm2[1]
	addsd	%xmm1, %xmm0
	addsd	%xmm2, %xmm0
	retq
.Lfunc_end1:
	.size	_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_, .Lfunc_end1-_Z14antidot_motorsRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj15ELi1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj0ELi1EEEEEEdEESB_
	.cfi_endproc
                                        # -- End function
	.globl	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE # -- Begin function _Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end2, nop
	.type	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE,@function
_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE: # @_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	24(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movsd	40(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movabsq	$-9223372036854775808, %rcx     # imm = 0x8000000000000000
	xorq	32(%rsi), %rcx
	movsd	%xmm1, (%rdi)
	movq	%rcx, 8(%rdi)
	movsd	%xmm0, 16(%rdi)
	retq
.Lfunc_end2:
	.size	_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE, .Lfunc_end2-_Z9bulk_lineRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S6_tlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
.LCPI3_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end3, nop
	.type	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE,@function
_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE: # @_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	movsd	24(%rdi), %xmm0                 # xmm0 = mem[0],zero
	xorps	.LCPI3_0(%rip), %xmm0
	retq
.Lfunc_end3:
	.size	_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE, .Lfunc_end3-_Z17weight_dual_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
.LCPI4_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
	.prefalign	4, .Lfunc_end4, nop
	.type	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE,@function
_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE: # @_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm4                   # xmm4 = mem[0],zero
	movsd	8(%rdx), %xmm2                  # xmm2 = mem[0],zero
	movsd	16(%rdx), %xmm0                 # xmm0 = mem[0],zero
	movsd	(%rsi), %xmm3                   # xmm3 = mem[0],zero
	movsd	8(%rsi), %xmm6                  # xmm6 = mem[0],zero
	movapd	%xmm4, %xmm1
	mulsd	%xmm6, %xmm1
	movapd	%xmm2, %xmm5
	mulsd	%xmm3, %xmm5
	subsd	%xmm5, %xmm1
	movsd	16(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm4, %xmm8
	mulsd	%xmm5, %xmm8
	mulsd	%xmm0, %xmm3
	subsd	%xmm3, %xmm8
	mulsd	%xmm2, %xmm5
	mulsd	%xmm0, %xmm6
	subsd	%xmm6, %xmm5
	movsd	24(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm4, %xmm9
	mulsd	%xmm3, %xmm9
	movapd	%xmm2, %xmm7
	mulsd	%xmm3, %xmm7
	mulsd	%xmm0, %xmm3
	movapd	%xmm4, %xmm6
	mulsd	%xmm1, %xmm6
	movapd	%xmm4, %xmm11
	mulsd	%xmm8, %xmm11
	mulsd	%xmm9, %xmm4
	xorpd	.LCPI4_0(%rip), %xmm1
	mulsd	%xmm2, %xmm1
	movapd	%xmm2, %xmm10
	mulsd	%xmm5, %xmm10
	addsd	%xmm11, %xmm10
	mulsd	%xmm7, %xmm2
	addsd	%xmm4, %xmm2
	mulsd	%xmm0, %xmm8
	subsd	%xmm8, %xmm1
	mulsd	%xmm0, %xmm5
	subsd	%xmm5, %xmm6
	mulsd	%xmm3, %xmm0
	addsd	%xmm2, %xmm0
	movsd	24(%rdx), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm9
	subsd	%xmm9, %xmm1
	mulsd	%xmm2, %xmm7
	subsd	%xmm7, %xmm6
	mulsd	%xmm2, %xmm3
	subsd	%xmm3, %xmm10
	movsd	%xmm1, (%rdi)
	movsd	%xmm6, 8(%rdi)
	movsd	%xmm10, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end4:
	.size	_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE, .Lfunc_end4-_Z19project_point_planeRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEERKNS1_IS2_XtlS4_tlS7_tlS8_tlS6_Lj14ELi1EEtlS6_Lj13ELin1EEtlS6_Lj11ELi1EEtlS6_Lj7ELin1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 23.1.4"
	.section	".note.GNU-stack","",@progbits
	.addrsig
