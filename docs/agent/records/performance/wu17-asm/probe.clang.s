	.att_syntax
	.file	"probe.cpp"
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_
.LCPI0_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_
	.prefalign	4, .Lfunc_end0, nop
	.type	_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_,@function
_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_: # @_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm8                   # xmm8 = mem[0],zero
	movsd	8(%rdx), %xmm6                  # xmm6 = mem[0],zero
	movsd	(%rsi), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rsi), %xmm7                  # xmm7 = mem[0],zero
	movapd	%xmm1, %xmm0
	mulsd	%xmm6, %xmm0
	movsd	16(%rdx), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm1, %xmm2
	mulsd	%xmm5, %xmm2
	movsd	24(%rdx), %xmm3                 # xmm3 = mem[0],zero
	mulsd	%xmm3, %xmm1
	movapd	%xmm7, %xmm4
	mulsd	%xmm8, %xmm4
	subsd	%xmm4, %xmm0
	movapd	%xmm5, %xmm4
	mulsd	%xmm7, %xmm4
	mulsd	%xmm3, %xmm7
	movsd	16(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm8, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm10, %xmm2
	movapd	%xmm6, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm10, %xmm4
	mulsd	%xmm9, %xmm3
	movsd	24(%rsi), %xmm9                 # xmm9 = mem[0],zero
	mulsd	%xmm9, %xmm8
	subsd	%xmm8, %xmm1
	mulsd	%xmm9, %xmm6
	subsd	%xmm6, %xmm7
	unpcklpd	%xmm7, %xmm1                    # xmm1 = xmm1[0],xmm7[0]
	mulsd	%xmm9, %xmm5
	subsd	%xmm5, %xmm3
	movapd	.LCPI0_0(%rip), %xmm5           # xmm5 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm5, %xmm2
	xorpd	%xmm5, %xmm3
	xorpd	%xmm5, %xmm1
	movupd	%xmm1, (%rdi)
	movlpd	%xmm3, 16(%rdi)
	movsd	%xmm4, 24(%rdi)
	movlpd	%xmm2, 32(%rdi)
	movsd	%xmm0, 40(%rdi)
	retq
.Lfunc_end0:
	.size	_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_, .Lfunc_end0-_Z17wedge_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_
.LCPI1_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_
	.prefalign	4, .Lfunc_end1, nop
	.type	_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_,@function
_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_: # @_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm8                   # xmm8 = mem[0],zero
	movsd	8(%rdx), %xmm6                  # xmm6 = mem[0],zero
	movsd	(%rsi), %xmm5                   # xmm5 = mem[0],zero
	movsd	8(%rsi), %xmm0                  # xmm0 = mem[0],zero
	movapd	%xmm5, %xmm3
	mulsd	%xmm6, %xmm3
	movsd	16(%rdx), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm5, %xmm2
	mulsd	%xmm7, %xmm2
	movsd	24(%rdx), %xmm1                 # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm5
	movapd	%xmm0, %xmm4
	mulsd	%xmm8, %xmm4
	subsd	%xmm4, %xmm3
	movapd	%xmm7, %xmm4
	mulsd	%xmm0, %xmm4
	mulsd	%xmm1, %xmm0
	movsd	16(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm8, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm10, %xmm2
	movapd	%xmm6, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm10, %xmm4
	mulsd	%xmm9, %xmm1
	movsd	24(%rsi), %xmm9                 # xmm9 = mem[0],zero
	mulsd	%xmm9, %xmm8
	subsd	%xmm8, %xmm5
	mulsd	%xmm9, %xmm6
	subsd	%xmm6, %xmm0
	mulsd	%xmm9, %xmm7
	subsd	%xmm7, %xmm1
	movsd	16(%rcx), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm3, %xmm9
	mulsd	%xmm7, %xmm9
	movsd	24(%rcx), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm10, %xmm3
	movsd	(%rcx), %xmm6                   # xmm6 = mem[0],zero
	movsd	8(%rcx), %xmm8                  # xmm8 = mem[0],zero
	movapd	%xmm2, %xmm11
	mulsd	%xmm8, %xmm11
	subsd	%xmm11, %xmm9
	mulsd	%xmm10, %xmm2
	movapd	%xmm7, %xmm11
	mulsd	%xmm5, %xmm11
	mulsd	%xmm8, %xmm5
	subsd	%xmm5, %xmm3
	subsd	%xmm11, %xmm2
	movapd	%xmm4, %xmm5
	mulsd	%xmm6, %xmm5
	addsd	%xmm9, %xmm5
	mulsd	%xmm10, %xmm4
	mulsd	%xmm0, %xmm7
	mulsd	%xmm6, %xmm0
	addsd	%xmm3, %xmm0
	subsd	%xmm7, %xmm4
	mulsd	%xmm1, %xmm6
	addsd	%xmm2, %xmm6
	mulsd	%xmm8, %xmm1
	addsd	%xmm4, %xmm1
	movapd	.LCPI1_0(%rip), %xmm2           # xmm2 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm2, %xmm5
	xorpd	%xmm2, %xmm6
	movsd	%xmm1, (%rdi)
	movlpd	%xmm6, 8(%rdi)
	movsd	%xmm0, 16(%rdi)
	movlpd	%xmm5, 24(%rdi)
	retq
.Lfunc_end1:
	.size	_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_, .Lfunc_end1-_Z23wedge_point_point_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEESD_SD_
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE
.LCPI2_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE
	.prefalign	4, .Lfunc_end2, nop
	.type	_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE,@function
_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE: # @_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rdx), %xmm2                  # xmm2 = mem[0],zero
	movapd	.LCPI2_0(%rip), %xmm0           # xmm0 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm1, %xmm0
	movsd	(%rsi), %xmm3                   # xmm3 = mem[0],zero
	movsd	8(%rsi), %xmm5                  # xmm5 = mem[0],zero
	mulsd	%xmm3, %xmm0
	movsd	24(%rdx), %xmm6                 # xmm6 = mem[0],zero
	mulsd	%xmm6, %xmm3
	movapd	%xmm6, %xmm4
	mulsd	%xmm5, %xmm4
	mulsd	%xmm2, %xmm5
	subsd	%xmm5, %xmm0
	movsd	16(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movsd	16(%rdx), %xmm5                 # xmm5 = mem[0],zero
	mulsd	%xmm7, %xmm6
	mulsd	%xmm5, %xmm7
	subsd	%xmm7, %xmm0
	movsd	24(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm2, %xmm8
	mulsd	%xmm7, %xmm8
	addsd	%xmm6, %xmm8
	mulsd	%xmm5, %xmm7
	subsd	%xmm7, %xmm4
	movsd	32(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm1, %xmm7
	mulsd	%xmm6, %xmm7
	subsd	%xmm7, %xmm8
	mulsd	%xmm6, %xmm5
	addsd	%xmm3, %xmm5
	movsd	40(%rsi), %xmm3                 # xmm3 = mem[0],zero
	mulsd	%xmm3, %xmm1
	addsd	%xmm4, %xmm1
	mulsd	%xmm3, %xmm2
	subsd	%xmm2, %xmm5
	movsd	%xmm5, (%rdi)
	movsd	%xmm1, 8(%rdi)
	movsd	%xmm8, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end2:
	.size	_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE, .Lfunc_end2-_Z20antiwedge_line_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm4EEEtlS7_IS8_Lm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.globl	_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE # -- Begin function _Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end3, nop
	.type	_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE,@function
_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE: # @_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	jmp	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE # TAILCALL
.Lfunc_end3:
	.size	_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE, .Lfunc_end3-_Z15transform_pointRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI4_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end4, nop
	.type	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,@function
_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE: # @_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm5                   # xmm5 = mem[0],zero
	movsd	8(%rsi), %xmm8                  # xmm8 = mem[0],zero
	movsd	(%rdx), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rdx), %xmm6                  # xmm6 = mem[0],zero
	movapd	%xmm1, %xmm2
	mulsd	%xmm5, %xmm2
	movapd	%xmm1, %xmm7
	mulsd	%xmm8, %xmm7
	movsd	16(%rsi), %xmm10                # xmm10 = mem[0],zero
	movsd	24(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm1, %xmm3
	movsd	%xmm1, -48(%rsp)                # 8-byte Spill
	mulsd	%xmm0, %xmm3
	movsd	%xmm3, -32(%rsp)                # 8-byte Spill
	movapd	%xmm5, %xmm4
	movsd	%xmm6, -40(%rsp)                # 8-byte Spill
	mulsd	%xmm6, %xmm4
	subsd	%xmm4, %xmm7
	movapd	%xmm8, %xmm9
	mulsd	%xmm6, %xmm9
	addsd	%xmm2, %xmm9
	movapd	%xmm10, %xmm13
	mulsd	%xmm6, %xmm13
	movapd	.LCPI4_0(%rip), %xmm2           # xmm2 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm0, %xmm2
	movapd	%xmm2, -24(%rsp)                # 16-byte Spill
	movsd	16(%rdx), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm5, %xmm14
	mulsd	%xmm3, %xmm14
	movapd	%xmm1, %xmm6
	mulsd	%xmm10, %xmm6
	subsd	%xmm6, %xmm14
	movapd	%xmm8, %xmm6
	mulsd	%xmm3, %xmm6
	subsd	%xmm6, %xmm13
	movapd	%xmm10, %xmm6
	mulsd	%xmm3, %xmm6
	addsd	%xmm9, %xmm6
	movapd	%xmm0, %xmm9
	movsd	24(%rdx), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm5
	addsd	%xmm13, %xmm5
	mulsd	%xmm2, %xmm8
	addsd	%xmm14, %xmm8
	mulsd	%xmm2, %xmm10
	addsd	%xmm7, %xmm10
	movsd	32(%rdx), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm0, %xmm12
	mulsd	%xmm7, %xmm12
	addsd	%xmm5, %xmm12
	movsd	40(%rdx), %xmm1                 # xmm1 = mem[0],zero
	movsd	%xmm1, -56(%rsp)                # 8-byte Spill
	movapd	%xmm0, %xmm14
	mulsd	%xmm1, %xmm14
	addsd	%xmm8, %xmm14
	movsd	48(%rdx), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm0, %xmm15
	mulsd	%xmm8, %xmm15
	addsd	%xmm10, %xmm15
	movapd	%xmm0, %xmm13
	movsd	56(%rdx), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm10, %xmm0
	subsd	%xmm0, %xmm6
	movapd	%xmm3, %xmm4
	mulsd	%xmm12, %xmm4
	movapd	%xmm2, %xmm11
	mulsd	%xmm12, %xmm11
	movsd	-48(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm0
	mulsd	%xmm14, %xmm0
	movsd	-40(%rsp), %xmm5                # 8-byte Reload
                                        # xmm5 = mem[0],zero
	mulsd	%xmm5, %xmm12
	subsd	%xmm12, %xmm0
	movapd	%xmm3, %xmm12
	mulsd	%xmm14, %xmm12
	subsd	%xmm12, %xmm11
	mulsd	%xmm2, %xmm14
	addsd	%xmm4, %xmm14
	movapd	%xmm1, %xmm4
	movapd	%xmm1, %xmm12
	mulsd	%xmm15, %xmm4
	subsd	%xmm4, %xmm14
	movapd	%xmm5, %xmm1
	movapd	%xmm5, %xmm4
	mulsd	%xmm15, %xmm4
	addsd	%xmm11, %xmm4
	mulsd	%xmm2, %xmm15
	addsd	%xmm0, %xmm15
	mulsd	%xmm2, %xmm13
	movapd	%xmm13, %xmm0
	mulsd	%xmm7, %xmm0
	addsd	%xmm4, %xmm0
	movapd	%xmm13, %xmm4
	movsd	-56(%rsp), %xmm5                # 8-byte Reload
                                        # xmm5 = mem[0],zero
	mulsd	%xmm5, %xmm4
	addsd	%xmm14, %xmm4
	mulsd	%xmm13, %xmm2
	mulsd	%xmm8, %xmm13
	addsd	%xmm15, %xmm13
	movapd	%xmm12, %xmm14
	mulsd	%xmm6, %xmm12
	addsd	%xmm0, %xmm12
	movapd	%xmm1, %xmm0
	movapd	%xmm1, %xmm11
	mulsd	%xmm6, %xmm0
	addsd	%xmm4, %xmm0
	mulsd	%xmm3, %xmm6
	addsd	%xmm13, %xmm6
	mulsd	%xmm3, %xmm9
	mulsd	%xmm9, %xmm3
	addsd	%xmm2, %xmm3
	movapd	%xmm9, %xmm1
	mulsd	%xmm7, %xmm1
	addsd	%xmm0, %xmm1
	movapd	%xmm9, %xmm0
	mulsd	%xmm5, %xmm0
	subsd	%xmm0, %xmm12
	mulsd	%xmm10, %xmm9
	subsd	%xmm9, %xmm6
	movapd	-24(%rsp), %xmm2                # 16-byte Reload
	movapd	%xmm11, %xmm0
	mulsd	%xmm11, %xmm2
	mulsd	%xmm2, %xmm0
	subsd	%xmm0, %xmm3
	mulsd	%xmm2, %xmm7
	addsd	%xmm6, %xmm7
	movapd	%xmm2, %xmm0
	mulsd	%xmm8, %xmm0
	subsd	%xmm0, %xmm12
	mulsd	%xmm10, %xmm2
	addsd	%xmm1, %xmm2
	movapd	%xmm14, %xmm0
	movsd	-32(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm0
	addsd	%xmm3, %xmm0
	movapd	%xmm5, %xmm3
	mulsd	%xmm1, %xmm3
	addsd	%xmm7, %xmm3
	mulsd	%xmm1, %xmm8
	subsd	%xmm8, %xmm2
	mulsd	%xmm10, %xmm1
	subsd	%xmm1, %xmm12
	movsd	%xmm12, (%rdi)
	movsd	%xmm2, 8(%rdi)
	movsd	%xmm3, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end4:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end4-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.text
	.globl	_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE # -- Begin function _Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end5, nop
	.type	_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE,@function
_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE: # @_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	jmp	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE # TAILCALL
.Lfunc_end5:
	.size	_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE, .Lfunc_end5-_Z14transform_lineRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI6_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end6, nop
	.type	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,@function
_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE: # @_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_startproc
# %bb.0:
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$72, %rsp
	.cfi_def_cfa_offset 96
	.cfi_offset %rbx, -24
	.cfi_offset %r14, -16
	movq	%rdx, %r14
	movq	%rsi, %rdx
	movq	%rdi, %rbx
	movq	%fs:40, %rax
	movq	%rax, 64(%rsp)
	movq	%rsp, %rdi
	movq	%r14, %rsi
	callq	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	movsd	(%r14), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%r14), %xmm0                  # xmm0 = mem[0],zero
	movapd	.LCPI6_0(%rip), %xmm5           # xmm5 = [-0.0E+0,-0.0E+0]
	movapd	%xmm0, %xmm9
	xorpd	%xmm5, %xmm9
	movsd	(%rsp), %xmm3                   # xmm3 = mem[0],zero
	movsd	8(%rsp), %xmm7                  # xmm7 = mem[0],zero
	mulsd	%xmm3, %xmm9
	movsd	16(%r14), %xmm2                 # xmm2 = mem[0],zero
	xorpd	%xmm2, %xmm5
	movapd	%xmm1, %xmm4
	mulsd	%xmm7, %xmm4
	subsd	%xmm4, %xmm9
	movapd	%xmm0, %xmm4
	mulsd	%xmm7, %xmm4
	movapd	%xmm3, %xmm6
	mulsd	%xmm1, %xmm6
	subsd	%xmm6, %xmm4
	movsd	24(%r14), %xmm8                 # xmm8 = mem[0],zero
	mulsd	%xmm8, %xmm7
	mulsd	%xmm2, %xmm3
	subsd	%xmm3, %xmm7
	movsd	16(%rsp), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm1, %xmm6
	mulsd	%xmm3, %xmm6
	subsd	%xmm6, %xmm7
	movapd	%xmm2, %xmm12
	mulsd	%xmm3, %xmm12
	addsd	%xmm4, %xmm12
	mulsd	%xmm8, %xmm3
	subsd	%xmm3, %xmm9
	movsd	24(%rsp), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm0, %xmm3
	mulsd	%xmm11, %xmm3
	mulsd	%xmm11, %xmm5
	movsd	40(%r14), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm11, %xmm4
	mulsd	%xmm6, %xmm4
	addsd	%xmm7, %xmm4
	movsd	48(%r14), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm11, %xmm10
	mulsd	%xmm7, %xmm10
	subsd	%xmm10, %xmm9
	movsd	56(%r14), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm11, %xmm13
	mulsd	%xmm10, %xmm13
	subsd	%xmm13, %xmm12
	movsd	32(%rsp), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm0, %xmm13
	mulsd	%xmm14, %xmm13
	subsd	%xmm13, %xmm4
	movapd	%xmm2, %xmm13
	mulsd	%xmm14, %xmm13
	addsd	%xmm9, %xmm13
	mulsd	%xmm8, %xmm14
	addsd	%xmm12, %xmm14
	movsd	40(%rsp), %xmm15                # xmm15 = mem[0],zero
	movapd	%xmm1, %xmm9
	mulsd	%xmm15, %xmm9
	subsd	%xmm9, %xmm3
	movapd	%xmm2, %xmm9
	mulsd	%xmm15, %xmm9
	mulsd	%xmm8, %xmm11
	subsd	%xmm11, %xmm9
	movapd	%xmm8, %xmm11
	mulsd	%xmm15, %xmm11
	subsd	%xmm11, %xmm5
	movsd	32(%r14), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm15, %xmm11
	mulsd	%xmm12, %xmm11
	subsd	%xmm11, %xmm4
	movapd	%xmm7, %xmm11
	mulsd	%xmm15, %xmm11
	addsd	%xmm14, %xmm11
	mulsd	%xmm10, %xmm15
	subsd	%xmm15, %xmm13
	movsd	48(%rsp), %xmm15                # xmm15 = mem[0],zero
	movapd	%xmm1, %xmm14
	mulsd	%xmm15, %xmm14
	addsd	%xmm5, %xmm14
	movapd	%xmm0, %xmm5
	mulsd	%xmm15, %xmm5
	subsd	%xmm5, %xmm9
	mulsd	%xmm15, %xmm8
	subsd	%xmm8, %xmm3
	movapd	%xmm12, %xmm5
	mulsd	%xmm15, %xmm5
	addsd	%xmm13, %xmm5
	movapd	%xmm6, %xmm8
	mulsd	%xmm15, %xmm8
	subsd	%xmm8, %xmm11
	mulsd	%xmm15, %xmm10
	subsd	%xmm10, %xmm4
	movsd	56(%rsp), %xmm8                 # xmm8 = mem[0],zero
	mulsd	%xmm8, %xmm1
	subsd	%xmm1, %xmm9
	mulsd	%xmm8, %xmm0
	subsd	%xmm0, %xmm14
	mulsd	%xmm8, %xmm2
	subsd	%xmm2, %xmm3
	mulsd	%xmm8, %xmm12
	subsd	%xmm12, %xmm11
	mulsd	%xmm8, %xmm6
	subsd	%xmm6, %xmm5
	mulsd	%xmm8, %xmm7
	subsd	%xmm7, %xmm4
	movsd	%xmm9, (%rbx)
	movsd	%xmm14, 8(%rbx)
	movsd	%xmm3, 16(%rbx)
	movsd	%xmm11, 24(%rbx)
	movsd	%xmm5, 32(%rbx)
	movsd	%xmm4, 40(%rbx)
	movq	%fs:40, %rax
	cmpq	64(%rsp), %rax
	jne	.LBB6_2
# %bb.1:
	movq	%rbx, %rax
	addq	$72, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	retq
.LBB6_2:
	.cfi_def_cfa_offset 96
	callq	__stack_chk_fail@PLT
.Lfunc_end6:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end6-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.text
	.globl	_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE # -- Begin function _Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end7, nop
	.type	_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE,@function
_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE: # @_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_startproc
# %bb.0:
	jmp	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE # TAILCALL
.Lfunc_end7:
	.size	_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE, .Lfunc_end7-_Z15transform_planeRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEEdEERKNS1_IS4_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI8_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end8, nop
	.type	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,@function
_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE: # @_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm0                   # xmm0 = mem[0],zero
	movapd	%xmm0, -56(%rsp)                # 16-byte Spill
	movsd	16(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm0, %xmm1
	mulsd	%xmm9, %xmm1
	movapd	%xmm1, %xmm2
	movsd	24(%rsi), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm0, %xmm5
	mulsd	%xmm14, %xmm5
	movupd	(%rsi), %xmm6
	movapd	%xmm0, %xmm1
	unpcklpd	%xmm0, %xmm1                    # xmm1 = xmm1[0],xmm0[0]
	movapd	%xmm1, -24(%rsp)                # 16-byte Spill
	movapd	%xmm6, %xmm8
	xorpd	.LCPI8_0(%rip), %xmm8
	shufpd	$2, %xmm6, %xmm8                # xmm8 = xmm8[0],xmm6[1]
	mulpd	%xmm1, %xmm8
	movupd	8(%rdx), %xmm3
	movapd	%xmm9, %xmm0
	mulsd	%xmm3, %xmm0
	movapd	%xmm3, %xmm1
	movapd	%xmm3, %xmm7
	movapd	%xmm3, -88(%rsp)                # 16-byte Spill
	unpcklpd	%xmm3, %xmm1                    # xmm1 = xmm1[0],xmm3[0]
	movapd	%xmm6, %xmm3
	shufpd	$1, %xmm6, %xmm3                # xmm3 = xmm3[1],xmm6[0]
	mulpd	%xmm1, %xmm3
	subpd	%xmm3, %xmm8
	movupd	16(%rdx), %xmm12
	movapd	%xmm6, %xmm1
	mulsd	%xmm12, %xmm1
	subsd	%xmm1, %xmm2
	movapd	%xmm6, %xmm10
	unpckhpd	%xmm6, %xmm10                   # xmm10 = xmm10[1],xmm6[1]
	movapd	%xmm10, %xmm1
	mulsd	%xmm12, %xmm1
	subsd	%xmm1, %xmm0
	movapd	%xmm12, %xmm13
	unpckhpd	%xmm12, %xmm13                  # xmm13 = xmm13[1],xmm12[1]
	movapd	%xmm6, %xmm11
	mulsd	%xmm13, %xmm11
	addsd	%xmm0, %xmm11
	movapd	%xmm10, %xmm0
	mulsd	%xmm13, %xmm0
	subsd	%xmm0, %xmm2
	movapd	%xmm2, -40(%rsp)                # 16-byte Spill
	movapd	%xmm9, %xmm4
	unpcklpd	%xmm9, %xmm4                    # xmm4 = xmm4[0],xmm9[0]
	mulpd	%xmm12, %xmm4
	movapd	%xmm8, %xmm0
	subpd	%xmm4, %xmm0
	movapd	%xmm0, -72(%rsp)                # 16-byte Spill
	addpd	%xmm8, %xmm4
	movupd	24(%rdx), %xmm8
	movapd	%xmm8, %xmm1
	unpckhpd	%xmm8, %xmm1                    # xmm1 = xmm1[1],xmm8[1]
	movapd	%xmm6, %xmm0
	mulsd	%xmm1, %xmm0
	movapd	%xmm14, %xmm2
	movapd	%xmm14, %xmm15
	mulsd	%xmm13, %xmm14
	subsd	%xmm14, %xmm0
	mulsd	%xmm12, %xmm15
	movapd	%xmm10, %xmm14
	mulsd	%xmm1, %xmm14
	addsd	%xmm15, %xmm14
	mulsd	%xmm7, %xmm2
	mulsd	%xmm9, %xmm1
	subsd	%xmm1, %xmm2
	movupd	40(%rdx), %xmm15
	movapd	%xmm6, %xmm1
	mulsd	%xmm15, %xmm1
	subsd	%xmm1, %xmm14
	movsd	56(%rdx), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm9, %xmm7
	mulsd	%xmm3, %xmm7
	addsd	%xmm7, %xmm14
	movapd	%xmm10, %xmm7
	mulsd	%xmm15, %xmm7
	addsd	%xmm0, %xmm7
	movapd	%xmm9, %xmm0
	mulsd	%xmm15, %xmm0
	addsd	%xmm5, %xmm0
	movsd	48(%rdx), %xmm1                 # xmm1 = mem[0],zero
	movapd	%xmm6, %xmm5
	mulsd	%xmm1, %xmm5
	addsd	%xmm2, %xmm5
	movapd	%xmm10, %xmm2
	mulsd	%xmm1, %xmm2
	subsd	%xmm2, %xmm0
	mulsd	%xmm1, %xmm9
	addsd	%xmm7, %xmm9
	mulsd	%xmm3, %xmm6
	addsd	%xmm0, %xmm6
	mulsd	%xmm3, %xmm10
	addsd	%xmm5, %xmm10
	movapd	-88(%rsp), %xmm1                # 16-byte Reload
	mulsd	%xmm1, %xmm10
	movapd	-56(%rsp), %xmm5                # 16-byte Reload
	mulsd	%xmm5, %xmm6
	addsd	%xmm6, %xmm10
	mulsd	%xmm12, %xmm14
	addsd	%xmm10, %xmm14
	movapd	-72(%rsp), %xmm7                # 16-byte Reload
	mulsd	%xmm7, %xmm3
	subsd	%xmm3, %xmm14
	mulsd	%xmm13, %xmm9
	subsd	%xmm9, %xmm14
	movapd	%xmm4, %xmm2
	unpckhpd	%xmm4, %xmm2                    # xmm2 = xmm2[1],xmm4[1]
	mulsd	%xmm13, %xmm2
	unpcklpd	%xmm14, %xmm2                   # xmm2 = xmm2[0],xmm14[0]
	movapd	%xmm4, %xmm0
	movsd	%xmm7, %xmm0                    # xmm0 = xmm7[0],xmm0[1]
	unpcklpd	%xmm15, %xmm5                   # xmm5 = xmm5[0],xmm15[0]
	movsd	%xmm12, %xmm15                  # xmm15 = xmm12[0],xmm15[1]
	mulpd	%xmm0, %xmm15
	subpd	%xmm15, %xmm2
	mulpd	-24(%rsp), %xmm0                # 16-byte Folded Reload
	movapd	.LCPI8_0(%rip), %xmm6           # xmm6 = [-0.0E+0,-0.0E+0]
	movapd	%xmm1, %xmm3
	xorpd	%xmm1, %xmm6
	shufpd	$1, %xmm7, %xmm4                # xmm4 = xmm4[1],xmm7[0]
	movsd	%xmm8, %xmm1                    # xmm1 = xmm8[0],xmm1[1]
	movsd	%xmm3, %xmm8                    # xmm8 = xmm3[0],xmm8[1]
	unpcklpd	%xmm6, %xmm3                    # xmm3 = xmm3[0],xmm6[0]
	mulpd	%xmm4, %xmm3
	subpd	%xmm0, %xmm3
	movapd	-40(%rsp), %xmm4                # 16-byte Reload
	unpcklpd	%xmm4, %xmm4                    # xmm4 = xmm4[0,0]
	mulpd	%xmm4, %xmm12
	mulpd	%xmm5, %xmm4
	movapd	%xmm2, %xmm0
	addpd	%xmm4, %xmm0
	subpd	%xmm4, %xmm2
	shufpd	$2, %xmm0, %xmm2                # xmm2 = xmm2[0],xmm0[1]
	movapd	%xmm3, %xmm0
	addpd	%xmm12, %xmm0
	subpd	%xmm12, %xmm3
	movsd	%xmm0, %xmm3                    # xmm3 = xmm0[0],xmm3[1]
	unpcklpd	%xmm11, %xmm11                  # xmm11 = xmm11[0,0]
	mulpd	%xmm11, %xmm8
	subpd	%xmm8, %xmm2
	mulpd	%xmm1, %xmm11
	addpd	%xmm3, %xmm11
	movupd	%xmm11, (%rdi)
	movupd	%xmm2, 16(%rdi)
	retq
.Lfunc_end8:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end8-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_
.LCPI9_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.text
	.globl	_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_
	.prefalign	4, .Lfunc_end9, nop
	.type	_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_,@function
_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_: # @_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$80, %rsp
	.cfi_def_cfa_offset 96
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%fs:40, %rax
	movq	%rax, 72(%rsp)
	movq	%rsp, %rdi
	callq	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE
	movups	24(%rsp), %xmm0
	movhps	40(%rsp), %xmm0                 # xmm0 = xmm0[0,1],mem[0,1]
	movsd	32(%rsp), %xmm1                 # xmm1 = mem[0],zero
	xorps	.LCPI9_0(%rip), %xmm0
	movabsq	$-9223372036854775808, %rax     # imm = 0x8000000000000000
	movq	48(%rsp), %rcx
	xorq	%rax, %rcx
	xorq	16(%rsp), %rax
	movsd	56(%rsp), %xmm2                 # xmm2 = mem[0],zero
	movups	%xmm0, (%rbx)
	movq	%rcx, 16(%rbx)
	movsd	%xmm2, 24(%rbx)
	movsd	%xmm1, 32(%rbx)
	movq	%rax, 40(%rbx)
	movaps	(%rsp), %xmm0
	shufps	$78, %xmm0, %xmm0               # xmm0 = xmm0[2,3,0,1]
	movups	%xmm0, 48(%rbx)
	movq	%fs:40, %rax
	cmpq	72(%rsp), %rax
	jne	.LBB9_2
# %bb.1:
	movq	%rbx, %rax
	addq	$80, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB9_2:
	.cfi_def_cfa_offset 96
	callq	__stack_chk_fail@PLT
.Lfunc_end9:
	.size	_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_, .Lfunc_end9-_Z14compose_motorsRKN6galoot4core2mvINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEEdEESD_
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
.LCPI10_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,"axG",@progbits,_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,comdat
	.weak	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.prefalign	4, .Lfunc_end10, nop
	.type	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,@function
_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE: # @_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	24(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm6, %xmm1
	xorpd	.LCPI10_0(%rip), %xmm1
	movsd	32(%rdx), %xmm11                # xmm11 = mem[0],zero
	movsd	40(%rdx), %xmm12                # xmm12 = mem[0],zero
	movupd	(%rdx), %xmm0
	movsd	16(%rdx), %xmm4                 # xmm4 = mem[0],zero
	movupd	16(%rsi), %xmm3
	movsd	8(%rsi), %xmm2                  # xmm2 = mem[0],zero
	movupd	(%rsi), %xmm10
	mulsd	%xmm10, %xmm1
	movapd	%xmm10, %xmm14
	mulsd	%xmm11, %xmm14
	movapd	%xmm10, %xmm13
	mulsd	%xmm12, %xmm13
	movapd	%xmm6, %xmm5
	mulsd	%xmm2, %xmm5
	subsd	%xmm5, %xmm14
	movapd	%xmm11, %xmm5
	mulsd	%xmm2, %xmm5
	subsd	%xmm5, %xmm1
	movapd	%xmm12, %xmm15
	mulsd	%xmm2, %xmm15
	movapd	%xmm0, %xmm8
	movapd	%xmm0, %xmm9
	movapd	%xmm0, -24(%rsp)                # 16-byte Spill
	unpckhpd	%xmm0, %xmm8                    # xmm8 = xmm8[1],xmm0[1]
	movapd	%xmm8, %xmm5
	mulsd	%xmm3, %xmm5
	movapd	%xmm4, %xmm7
	mulsd	%xmm2, %xmm7
	subsd	%xmm7, %xmm5
	movapd	%xmm6, %xmm7
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm13
	movapd	%xmm11, %xmm7
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm15
	movapd	%xmm12, %xmm7
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm1
	movsd	24(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm9, %xmm7
	mulsd	%xmm0, %xmm7
	subsd	%xmm7, %xmm5
	movsd	32(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movsd	40(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movsd	%xmm5, 24(%rdi)
	mulsd	%xmm0, %xmm6
	addsd	%xmm15, %xmm6
	mulsd	%xmm0, %xmm11
	subsd	%xmm11, %xmm13
	mulsd	%xmm0, %xmm12
	addsd	%xmm14, %xmm12
	movapd	-24(%rsp), %xmm5                # 16-byte Reload
	movapd	%xmm5, %xmm0
	mulsd	%xmm7, %xmm0
	subsd	%xmm0, %xmm1
	movapd	%xmm8, %xmm11
	mulsd	%xmm7, %xmm11
	addsd	%xmm12, %xmm11
	movapd	.LCPI10_0(%rip), %xmm14         # xmm14 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm5, %xmm14
	mulsd	%xmm4, %xmm7
	addsd	%xmm13, %xmm7
	movapd	%xmm5, %xmm0
	movapd	%xmm5, %xmm12
	mulsd	%xmm9, %xmm0
	subsd	%xmm0, %xmm11
	movapd	%xmm8, %xmm0
	mulsd	%xmm9, %xmm0
	subsd	%xmm0, %xmm1
	mulsd	%xmm4, %xmm9
	movapd	%xmm5, %xmm0
	unpcklpd	%xmm10, %xmm0                   # xmm0 = xmm0[0],xmm10[0]
	unpcklpd	%xmm14, %xmm2                   # xmm2 = xmm2[0],xmm14[0]
	mulpd	%xmm0, %xmm2
	movapd	%xmm8, %xmm5
	mulpd	%xmm10, %xmm5
	mulsd	%xmm4, %xmm10
	addsd	%xmm6, %xmm9
	unpcklpd	%xmm10, %xmm9                   # xmm9 = xmm9[0],xmm10[0]
	movupd	48(%rsi), %xmm6
	movapd	%xmm12, %xmm0
	mulsd	%xmm6, %xmm0
	subsd	%xmm0, %xmm7
	movapd	%xmm6, %xmm0
	movapd	%xmm4, %xmm10
	mulsd	%xmm6, %xmm10
	unpckhpd	%xmm3, %xmm6                    # xmm6 = xmm6[1],xmm3[1]
	mulpd	%xmm12, %xmm6
	shufpd	$1, %xmm12, %xmm12              # xmm12 = xmm12[1,0]
	unpcklpd	%xmm3, %xmm0                    # xmm0 = xmm0[0],xmm3[0]
	mulpd	%xmm12, %xmm0
	subpd	%xmm0, %xmm9
	subpd	%xmm5, %xmm2
	subsd	%xmm10, %xmm1
	movsd	56(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movsd	%xmm1, (%rdi)
	movapd	%xmm4, %xmm1
	unpcklpd	%xmm4, %xmm1                    # xmm1 = xmm1[0],xmm4[0]
	shufpd	$1, %xmm3, %xmm3                # xmm3 = xmm3[1,0]
	mulpd	%xmm1, %xmm3
	subpd	%xmm3, %xmm2
	movapd	%xmm9, %xmm1
	addpd	%xmm6, %xmm1
	subpd	%xmm6, %xmm9
	movsd	%xmm1, %xmm9                    # xmm9 = xmm1[0],xmm9[1]
	movupd	%xmm2, 48(%rdi)
	movupd	%xmm9, 32(%rdi)
	mulsd	%xmm0, %xmm8
	subsd	%xmm8, %xmm7
	movsd	%xmm7, 16(%rdi)
	mulsd	%xmm0, %xmm4
	addsd	%xmm11, %xmm4
	movsd	%xmm4, 8(%rdi)
	retq
.Lfunc_end10:
	.size	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE, .Lfunc_end10-_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE
.LCPI11_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0
.LCPI11_1:
	.quad	0x3ff0000000000000              # double 1
	.section	.text._ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE,"axG",@progbits,_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE,comdat
	.weak	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE
	.prefalign	4, .Lfunc_end11, nop
	.type	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE,@function
_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE: # @_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm11                  # xmm11 = mem[0],zero
	movapd	%xmm11, -40(%rsp)               # 16-byte Spill
	movapd	.LCPI11_0(%rip), %xmm0          # xmm0 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm0, %xmm11
	movsd	32(%rdx), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm10, %xmm14
	xorpd	%xmm0, %xmm14
	movsd	40(%rdx), %xmm7                 # xmm7 = mem[0],zero
	movsd	48(%rdx), %xmm3                 # xmm3 = mem[0],zero
	movsd	56(%rdx), %xmm15                # xmm15 = mem[0],zero
	movsd	8(%rsi), %xmm2                  # xmm2 = mem[0],zero
	movupd	(%rsi), %xmm9
	mulsd	%xmm9, %xmm11
	movupd	8(%rdx), %xmm12
	movapd	%xmm12, -72(%rsp)               # 16-byte Spill
	movsd	8(%rdx), %xmm4                  # xmm4 = mem[0],zero
	movsd	%xmm4, -88(%rsp)                # 8-byte Spill
	mulsd	%xmm9, %xmm14
	movapd	%xmm9, %xmm0
	mulsd	%xmm7, %xmm0
	movapd	%xmm9, %xmm5
	mulsd	%xmm3, %xmm5
	movapd	%xmm9, %xmm1
	mulsd	%xmm15, %xmm1
	mulsd	%xmm2, %xmm4
	subsd	%xmm4, %xmm11
	movapd	%xmm10, %xmm4
	mulsd	%xmm2, %xmm4
	subsd	%xmm4, %xmm0
	movapd	%xmm7, %xmm4
	movapd	%xmm7, %xmm6
	movsd	%xmm7, -80(%rsp)                # 8-byte Spill
	mulsd	%xmm2, %xmm4
	subsd	%xmm4, %xmm14
	movapd	%xmm3, %xmm13
	movapd	%xmm2, -56(%rsp)                # 16-byte Spill
	mulsd	%xmm2, %xmm13
	addsd	%xmm1, %xmm13
	movapd	%xmm15, %xmm1
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm5
	unpckhpd	%xmm12, %xmm12                  # xmm12 = xmm12[1,1]
	movupd	16(%rsi), %xmm7
	movapd	%xmm12, %xmm1
	mulsd	%xmm7, %xmm1
	subsd	%xmm1, %xmm11
	movapd	%xmm10, %xmm1
	mulsd	%xmm7, %xmm1
	subsd	%xmm1, %xmm5
	movapd	%xmm6, %xmm1
	mulsd	%xmm7, %xmm1
	subsd	%xmm1, %xmm13
	movapd	%xmm3, %xmm1
	mulsd	%xmm7, %xmm1
	subsd	%xmm1, %xmm14
	movapd	%xmm15, %xmm4
	mulsd	%xmm7, %xmm4
	addsd	%xmm0, %xmm4
	movsd	.LCPI11_1(%rip), %xmm0          # xmm0 = [1.0E+0,0.0E+0]
	movapd	%xmm7, %xmm1
	shufpd	$1, %xmm0, %xmm1                # xmm1 = xmm1[1],xmm0[0]
	movapd	%xmm1, -24(%rsp)                # 16-byte Spill
	movsd	24(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movsd	24(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm6, %xmm8
	mulsd	%xmm1, %xmm8
	addsd	%xmm11, %xmm8
	movsd	32(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movsd	40(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movsd	(%rdx), %xmm11                  # xmm11 = mem[0],zero
	movsd	%xmm8, 56(%rdi)
	mulsd	%xmm1, %xmm10
	addsd	%xmm13, %xmm10
	movsd	-80(%rsp), %xmm8                # 8-byte Reload
                                        # xmm8 = mem[0],zero
	mulsd	%xmm1, %xmm8
	subsd	%xmm8, %xmm5
	mulsd	%xmm1, %xmm3
	addsd	%xmm4, %xmm3
	mulsd	%xmm1, %xmm15
	addsd	%xmm14, %xmm15
	movapd	-40(%rsp), %xmm4                # 16-byte Reload
	movapd	%xmm4, %xmm1
	mulsd	%xmm0, %xmm1
	subsd	%xmm1, %xmm15
	movsd	-88(%rsp), %xmm8                # 8-byte Reload
                                        # xmm8 = mem[0],zero
	movapd	%xmm8, %xmm14
	mulsd	%xmm0, %xmm14
	addsd	%xmm3, %xmm14
	movapd	%xmm4, %xmm1
	movapd	%xmm4, %xmm3
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm14
	movapd	%xmm12, %xmm13
	mulsd	%xmm0, %xmm13
	addsd	%xmm5, %xmm13
	mulsd	%xmm6, %xmm0
	addsd	%xmm10, %xmm0
	movapd	%xmm8, %xmm1
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm15
	movapd	-72(%rsp), %xmm10               # 16-byte Reload
	movapd	%xmm10, %xmm5
	shufpd	$1, -56(%rsp), %xmm5            # 16-byte Folded Reload
                                        # xmm5 = xmm5[1],mem[0]
	movapd	%xmm9, %xmm1
	unpcklpd	%xmm4, %xmm1                    # xmm1 = xmm1[0],xmm4[0]
	mulpd	%xmm1, %xmm5
	movapd	%xmm12, %xmm1
	mulsd	%xmm2, %xmm1
	addsd	%xmm0, %xmm1
	movapd	.LCPI11_0(%rip), %xmm8          # xmm8 = [-0.0E+0,-0.0E+0]
	xorpd	%xmm6, %xmm8
	mulsd	%xmm9, %xmm8
	unpcklpd	%xmm1, %xmm8                    # xmm8 = xmm8[0],xmm1[0]
	movapd	%xmm6, %xmm4
	movapd	%xmm10, %xmm1
	unpcklpd	%xmm10, %xmm4                   # xmm4 = xmm4[0],xmm10[0]
	mulsd	%xmm6, %xmm2
	subsd	%xmm2, %xmm13
	movapd	%xmm7, %xmm10
	unpckhpd	%xmm7, %xmm10                   # xmm10 = xmm10[1],xmm7[1]
	mulpd	%xmm1, %xmm10
	movapd	%xmm1, %xmm0
	unpcklpd	%xmm3, %xmm1                    # xmm1 = xmm1[0],xmm3[0]
	movapd	%xmm3, %xmm2
	movupd	48(%rsi), %xmm3
	mulsd	%xmm3, %xmm2
	subsd	%xmm2, %xmm13
	shufpd	$1, %xmm0, %xmm0                # xmm0 = xmm0[1,0]
	movapd	%xmm9, %xmm2
	shufpd	$1, %xmm3, %xmm2                # xmm2 = xmm2[1],xmm3[0]
	mulpd	%xmm0, %xmm2
	subpd	%xmm2, %xmm8
	shufpd	$1, %xmm9, %xmm9                # xmm9 = xmm9[1,0]
	mulpd	%xmm4, %xmm9
	subpd	%xmm9, %xmm5
	movapd	%xmm12, %xmm0
	mulsd	%xmm3, %xmm0
	subsd	%xmm0, %xmm15
	movapd	%xmm6, %xmm0
	mulsd	%xmm3, %xmm0
	addsd	%xmm14, %xmm0
	movsd	%xmm7, %xmm3                    # xmm3 = xmm7[0],xmm3[1]
	mulpd	%xmm1, %xmm3
	addpd	%xmm8, %xmm3
	movapd	-24(%rsp), %xmm1                # 16-byte Reload
	mulpd	%xmm11, %xmm1
	subpd	%xmm1, %xmm3
	unpcklpd	%xmm6, %xmm11                   # xmm11 = xmm11[0],xmm6[0]
	unpcklpd	%xmm7, %xmm7                    # xmm7 = xmm7[0,0]
	mulpd	%xmm11, %xmm7
	subpd	%xmm7, %xmm5
	subpd	%xmm10, %xmm5
	movsd	56(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movupd	%xmm5, 40(%rdi)
	movupd	%xmm3, 24(%rdi)
	movsd	-88(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	mulsd	%xmm1, %xmm2
	subsd	%xmm2, %xmm13
	movsd	%xmm13, 16(%rdi)
	mulsd	%xmm1, %xmm12
	addsd	%xmm0, %xmm12
	movsd	%xmm12, 8(%rdi)
	mulsd	%xmm1, %xmm6
	addsd	%xmm15, %xmm6
	movsd	%xmm6, (%rdi)
	retq
.Lfunc_end11:
	.size	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE, .Lfunc_end11-_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_14product_kernelILNS1_15BinaryOperationE2ENS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SD_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj9ELin1EEtlSD_Lj10ELin1EEtlSD_Lj12ELin1EEtlSD_Lj15ELi1EEtlSD_Lj6ELi1EEtlSD_Lj5ELin1EEtlSD_Lj3ELi1EEtlSD_Lj0ELi1EEEEEEXtlSB_tlSE_tlSF_tlSD_Lj0ELi1EEtlSD_Lj3ELi1EEtlSD_Lj5ELi1EEtlSD_Lj9ELi1EEtlSD_Lj6ELi1EEtlSD_Lj10ELi1EEtlSD_Lj12ELi1EEtlSD_Lj15ELi1EEEEEEEEELm8EdLm8ELm8EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35ELm36ELm37ELm38ELm39ELm40ELm41ELm42ELm43ELm44ELm45ELm46ELm47EEEESC_IT1_XT0_EERKSC_ISG_XT2_EERKSC_ISG_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 23.1.4"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __stack_chk_fail
