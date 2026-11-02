	.intel_syntax noprefix
	.file	"kernels.cpp"
	.text
	.globl	_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_ # -- Begin function _Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
	.prefalign	4, .Lfunc_end0, nop
	.type	_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_,@function
_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_: # @_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
# %bb.0:
	mov	rax, rdi
	movsd	xmm5, qword ptr [rdx + 16]      # xmm5 = mem[0],zero
	movupd	xmm6, xmmword ptr [rdx]
	movupd	xmm2, xmmword ptr [rdx + 8]
	movapd	xmm0, xmm6
	unpcklpd	xmm0, xmm6                      # xmm0 = xmm0[0],xmm6[0]
	movupd	xmm4, xmmword ptr [rsi]
	movupd	xmm1, xmmword ptr [rsi + 8]
	movapd	xmm3, xmm4
	mulsd	xmm3, xmm6
	movsd	xmm7, qword ptr [rsi + 24]      # xmm7 = mem[0],zero
	mulpd	xmm0, xmm1
	movsd	xmm1, xmm7                      # xmm1 = xmm7[0],xmm1[1]
	mulpd	xmm1, xmm6
	shufpd	xmm6, xmm5, 1                   # xmm6 = xmm6[1],xmm5[0]
	movsd	xmm5, qword ptr [rsi + 16]      # xmm5 = mem[0],zero
	movapd	xmm8, xmm4
	unpcklpd	xmm8, xmm4                      # xmm8 = xmm8[0],xmm4[0]
	movsd	xmm9, qword ptr [rdx + 24]      # xmm9 = mem[0],zero
	movapd	xmm10, xmm2
	movsd	xmm11, qword ptr [rsi + 8]      # xmm11 = mem[0],zero
	mulsd	xmm11, xmm2
	mulpd	xmm8, xmm2
	movsd	xmm2, xmm9                      # xmm2 = xmm9[0],xmm2[1]
	mulpd	xmm2, xmm4
	shufpd	xmm4, xmm5, 1                   # xmm4 = xmm4[1],xmm5[0]
	unpcklpd	xmm9, xmm9                      # xmm9 = xmm9[0,0]
	mulpd	xmm9, xmm4
	unpckhpd	xmm10, xmm10                    # xmm10 = xmm10[1,1]
	unpcklpd	xmm7, xmm7                      # xmm7 = xmm7[0,0]
	mulpd	xmm7, xmm6
	subpd	xmm9, xmm7
	addsd	xmm11, xmm3
	mulsd	xmm10, xmm5
	addsd	xmm10, xmm11
	subpd	xmm8, xmm0
	subpd	xmm2, xmm1
	movsd	qword ptr [rdi], xmm10
	movupd	xmmword ptr [rdi + 8], xmm8
	movupd	xmmword ptr [rdi + 24], xmm2
	movupd	xmmword ptr [rdi + 40], xmm9
	ret
.Lfunc_end0:
	.size	_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_, .Lfunc_end0-_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
                                        # -- End function
	.globl	_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_ # -- Begin function _Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
	.prefalign	4, .Lfunc_end1, nop
	.type	_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_,@function
_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_: # @_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
# %bb.0:
	mov	rax, rdi
	movupd	xmm4, xmmword ptr [rdx]
	movupd	xmm1, xmmword ptr [rdx + 8]
	movsd	xmm3, qword ptr [rdx + 24]      # xmm3 = mem[0],zero
	movapd	xmm0, xmm1
	movsd	xmm0, xmm3                      # xmm0 = xmm3[0],xmm0[1]
	unpcklpd	xmm3, xmm3                      # xmm3 = xmm3[0,0]
	movsd	xmm6, qword ptr [rdx + 16]      # xmm6 = mem[0],zero
	movapd	xmm2, xmm4
	unpcklpd	xmm2, xmm4                      # xmm2 = xmm2[0],xmm4[0]
	movupd	xmm5, xmmword ptr [rsi + 8]
	movsd	xmm7, qword ptr [rsi + 24]      # xmm7 = mem[0],zero
	mulpd	xmm2, xmm5
	movsd	xmm5, xmm7                      # xmm5 = xmm7[0],xmm5[1]
	mulpd	xmm5, xmm4
	shufpd	xmm4, xmm6, 1                   # xmm4 = xmm4[1],xmm6[0]
	movsd	xmm6, qword ptr [rsi + 16]      # xmm6 = mem[0],zero
	movupd	xmm8, xmmword ptr [rsi]
	movapd	xmm9, xmm8
	shufpd	xmm9, xmm6, 1                   # xmm9 = xmm9[1],xmm6[0]
	mulpd	xmm9, xmm3
	unpcklpd	xmm7, xmm7                      # xmm7 = xmm7[0,0]
	mulpd	xmm7, xmm4
	subpd	xmm9, xmm7
	mulpd	xmm0, xmm8
	unpcklpd	xmm8, xmm8                      # xmm8 = xmm8[0,0]
	mulpd	xmm8, xmm1
	subpd	xmm0, xmm5
	subpd	xmm8, xmm2
	movupd	xmmword ptr [rdi + 16], xmm0
	movupd	xmmword ptr [rdi + 32], xmm9
	movupd	xmmword ptr [rdi], xmm8
	ret
.Lfunc_end1:
	.size	_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_, .Lfunc_end1-_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_
                                        # -- End function
	.globl	_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE # -- Begin function _Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
	.prefalign	4, .Lfunc_end2, nop
	.type	_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE,@function
_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE: # @_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
# %bb.0:
	mov	rax, rdi
	movsd	xmm3, qword ptr [rdx]           # xmm3 = mem[0],zero
	movsd	xmm0, qword ptr [rdx + 8]       # xmm0 = mem[0],zero
	movsd	xmm6, qword ptr [rsi]           # xmm6 = mem[0],zero
	movsd	xmm2, qword ptr [rsi + 8]       # xmm2 = mem[0],zero
	movapd	xmm5, xmm6
	mulsd	xmm5, xmm3
	movapd	xmm11, xmm6
	mulsd	xmm11, xmm0
	movsd	xmm4, qword ptr [rdx + 16]      # xmm4 = mem[0],zero
	movapd	xmm9, xmm6
	mulsd	xmm9, xmm4
	movsd	xmm1, qword ptr [rdx + 24]      # xmm1 = mem[0],zero
	mulsd	xmm6, xmm1
	movapd	xmm8, xmm3
	mulsd	xmm8, xmm2
	addsd	xmm8, xmm6
	movapd	xmm7, xmm0
	mulsd	xmm7, xmm2
	mulsd	xmm2, xmm4
	movsd	xmm12, qword ptr [rsi + 16]     # xmm12 = mem[0],zero
	movapd	xmm6, xmm3
	mulsd	xmm6, xmm12
	subsd	xmm7, xmm6
	movapd	xmm10, xmm0
	mulsd	xmm10, xmm12
	addsd	xmm10, xmm8
	mulsd	xmm12, xmm4
	movsd	xmm6, qword ptr [rsi + 24]      # xmm6 = mem[0],zero
	movapd	xmm8, xmm3
	mulsd	xmm8, xmm6
	subsd	xmm2, xmm8
	movapd	xmm8, xmm0
	mulsd	xmm8, xmm6
	subsd	xmm12, xmm8
	mulsd	xmm6, xmm4
	addsd	xmm6, xmm10
	movsd	xmm8, qword ptr [rsi + 32]      # xmm8 = mem[0],zero
	movapd	xmm13, xmm3
	mulsd	xmm13, xmm8
	movapd	xmm10, xmm0
	mulsd	xmm10, xmm8
	subsd	xmm9, xmm10
	movapd	xmm10, xmm4
	mulsd	xmm10, xmm8
	addsd	xmm10, xmm11
	mulsd	xmm8, xmm1
	addsd	xmm8, xmm12
	movsd	xmm12, qword ptr [rsi + 40]     # xmm12 = mem[0],zero
	movapd	xmm11, xmm3
	mulsd	xmm11, xmm12
	addsd	xmm11, xmm9
	movapd	xmm9, xmm0
	mulsd	xmm9, xmm12
	addsd	xmm9, xmm13
	movapd	xmm13, xmm4
	mulsd	xmm13, xmm12
	subsd	xmm5, xmm13
	mulsd	xmm12, xmm1
	subsd	xmm2, xmm12
	movsd	xmm12, qword ptr [rsi + 48]     # xmm12 = mem[0],zero
	movapd	xmm13, xmm3
	mulsd	xmm13, xmm12
	subsd	xmm10, xmm13
	movapd	xmm13, xmm0
	mulsd	xmm13, xmm12
	addsd	xmm13, xmm5
	movapd	xmm5, xmm4
	mulsd	xmm5, xmm12
	addsd	xmm5, xmm9
	mulsd	xmm1, xmm12
	addsd	xmm1, xmm7
	movsd	xmm7, qword ptr [rsi + 56]      # xmm7 = mem[0],zero
	mulsd	xmm3, xmm7
	subsd	xmm8, xmm3
	mulsd	xmm0, xmm7
	addsd	xmm0, xmm2
	mulsd	xmm4, xmm7
	subsd	xmm1, xmm4
	movsd	qword ptr [rdi], xmm13
	movsd	qword ptr [rdi + 8], xmm10
	movsd	qword ptr [rdi + 16], xmm11
	movsd	qword ptr [rdi + 24], xmm6
	movsd	qword ptr [rdi + 32], xmm5
	movsd	qword ptr [rdi + 40], xmm1
	movsd	qword ptr [rdi + 48], xmm0
	movsd	qword ptr [rdi + 56], xmm8
	ret
.Lfunc_end2:
	.size	_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE, .Lfunc_end2-_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE
                                        # -- End function
	.globl	_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_ # -- Begin function _Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_
	.prefalign	4, .Lfunc_end3, nop
	.type	_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_,@function
_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_: # @_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_
# %bb.0:
	mov	rax, rdi
	movsd	xmm2, qword ptr [rsi + 16]      # xmm2 = mem[0],zero
	movupd	xmm6, xmmword ptr [rsi]
	movupd	xmm0, xmmword ptr [rsi + 8]
	movapd	xmm1, xmm6
	unpcklpd	xmm1, xmm6                      # xmm1 = xmm1[0],xmm6[0]
	movupd	xmm7, xmmword ptr [rdx + 8]
	movapd	xmm4, xmm6
	movsd	xmm3, qword ptr [rdx + 24]      # xmm3 = mem[0],zero
	movapd	xmm5, xmm7
	movsd	xmm8, qword ptr [rsi + 8]       # xmm8 = mem[0],zero
	mulsd	xmm8, xmm7
	mulpd	xmm1, xmm7
	movsd	xmm7, xmm3                      # xmm7 = xmm3[0],xmm7[1]
	mulpd	xmm7, xmm6
	shufpd	xmm6, xmm2, 1                   # xmm6 = xmm6[1],xmm2[0]
	movsd	xmm9, qword ptr [rdx + 16]      # xmm9 = mem[0],zero
	movupd	xmm10, xmmword ptr [rdx]
	movapd	xmm11, xmm10
	unpcklpd	xmm11, xmm10                    # xmm11 = xmm11[0],xmm10[0]
	mulsd	xmm4, xmm10
	movsd	xmm12, qword ptr [rsi + 24]     # xmm12 = mem[0],zero
	mulpd	xmm11, xmm0
	movsd	xmm0, xmm12                     # xmm0 = xmm12[0],xmm0[1]
	mulpd	xmm0, xmm10
	shufpd	xmm10, xmm9, 1                  # xmm10 = xmm10[1],xmm9[0]
	unpcklpd	xmm3, xmm3                      # xmm3 = xmm3[0,0]
	mulpd	xmm3, xmm6
	unpckhpd	xmm5, xmm5                      # xmm5 = xmm5[1,1]
	unpcklpd	xmm12, xmm12                    # xmm12 = xmm12[0,0]
	mulpd	xmm12, xmm10
	subpd	xmm3, xmm12
	addsd	xmm8, xmm4
	mulsd	xmm5, xmm2
	addsd	xmm5, xmm8
	movsd	qword ptr [rdi], xmm5
	subpd	xmm1, xmm11
	subpd	xmm7, xmm0
	movupd	xmmword ptr [rdi + 8], xmm1
	movupd	xmmword ptr [rdi + 24], xmm7
	movupd	xmmword ptr [rdi + 40], xmm3
	ret
.Lfunc_end3:
	.size	_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_, .Lfunc_end3-_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_
                                        # -- End function
	.globl	_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_ # -- Begin function _Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_
	.prefalign	4, .Lfunc_end4, nop
	.type	_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_,@function
_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_: # @_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_
# %bb.0:
	mov	rax, rdi
	movsd	xmm3, qword ptr [rdx + 16]      # xmm3 = mem[0],zero
	movupd	xmm4, xmmword ptr [rdx]
	movupd	xmm0, xmmword ptr [rdx + 8]
	movapd	xmm1, xmm4
	unpcklpd	xmm1, xmm4                      # xmm1 = xmm1[0],xmm4[0]
	movupd	xmm2, xmmword ptr [rsi + 8]
	movsd	xmm5, qword ptr [rsi + 24]      # xmm5 = mem[0],zero
	mulpd	xmm1, xmm2
	movsd	xmm2, xmm5                      # xmm2 = xmm5[0],xmm2[1]
	mulpd	xmm2, xmm4
	shufpd	xmm4, xmm3, 1                   # xmm4 = xmm4[1],xmm3[0]
	movsd	xmm3, qword ptr [rsi + 16]      # xmm3 = mem[0],zero
	movupd	xmm6, xmmword ptr [rsi]
	movapd	xmm7, xmm6
	unpcklpd	xmm7, xmm6                      # xmm7 = xmm7[0],xmm6[0]
	movsd	xmm8, qword ptr [rdx + 24]      # xmm8 = mem[0],zero
	mulpd	xmm7, xmm0
	movsd	xmm0, xmm8                      # xmm0 = xmm8[0],xmm0[1]
	mulpd	xmm0, xmm6
	shufpd	xmm6, xmm3, 1                   # xmm6 = xmm6[1],xmm3[0]
	unpcklpd	xmm8, xmm8                      # xmm8 = xmm8[0,0]
	mulpd	xmm8, xmm6
	unpcklpd	xmm5, xmm5                      # xmm5 = xmm5[0,0]
	mulpd	xmm5, xmm4
	subpd	xmm8, xmm5
	subpd	xmm0, xmm2
	subpd	xmm7, xmm1
	movupd	xmmword ptr [rdi + 16], xmm0
	movupd	xmmword ptr [rdi + 32], xmm8
	movupd	xmmword ptr [rdi], xmm7
	ret
.Lfunc_end4:
	.size	_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_, .Lfunc_end4-_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_
                                        # -- End function
	.globl	_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end5, nop
	.type	_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE
# %bb.0:
	mov	rax, rdi
	movsd	xmm9, qword ptr [rsi]           # xmm9 = mem[0],zero
	movsd	xmm4, qword ptr [rsi + 40]      # xmm4 = mem[0],zero
	movsd	xmm0, qword ptr [rdx]           # xmm0 = mem[0],zero
	movsd	xmm2, qword ptr [rdx + 8]       # xmm2 = mem[0],zero
	movapd	xmm3, xmm9
	mulsd	xmm3, xmm0
	movsd	xmm1, qword ptr [rdx + 16]      # xmm1 = mem[0],zero
	movapd	xmm5, xmm4
	mulsd	xmm5, xmm1
	subsd	xmm3, xmm5
	movsd	xmm5, qword ptr [rsi + 48]      # xmm5 = mem[0],zero
	movapd	xmm6, xmm5
	mulsd	xmm6, xmm2
	addsd	xmm6, xmm3
	movsd	qword ptr [rdi], xmm6
	movapd	xmm6, xmm9
	mulsd	xmm6, xmm2
	movsd	xmm3, qword ptr [rsi + 32]      # xmm3 = mem[0],zero
	movapd	xmm7, xmm1
	mulsd	xmm7, xmm3
	addsd	xmm7, xmm6
	movapd	xmm6, xmm0
	mulsd	xmm6, xmm5
	subsd	xmm7, xmm6
	movsd	qword ptr [rdi + 8], xmm7
	movapd	xmm6, xmm9
	mulsd	xmm6, xmm1
	movapd	xmm7, xmm2
	mulsd	xmm7, xmm3
	subsd	xmm6, xmm7
	movapd	xmm7, xmm0
	mulsd	xmm7, xmm4
	addsd	xmm7, xmm6
	movsd	qword ptr [rdi + 16], xmm7
	movsd	xmm6, qword ptr [rdx + 24]      # xmm6 = mem[0],zero
	mulsd	xmm9, xmm6
	movsd	xmm8, qword ptr [rsi + 8]       # xmm8 = mem[0],zero
	movsd	xmm7, qword ptr [rsi + 16]      # xmm7 = mem[0],zero
	movapd	xmm10, xmm0
	mulsd	xmm10, xmm8
	addsd	xmm10, xmm9
	movapd	xmm11, xmm2
	mulsd	xmm11, xmm7
	addsd	xmm11, xmm10
	movsd	xmm9, qword ptr [rsi + 24]      # xmm9 = mem[0],zero
	movapd	xmm10, xmm1
	mulsd	xmm10, xmm9
	addsd	xmm10, xmm11
	movsd	qword ptr [rdi + 24], xmm10
	movapd	xmm10, xmm0
	mulsd	xmm10, xmm3
	movapd	xmm11, xmm4
	mulsd	xmm11, xmm2
	addsd	xmm11, xmm10
	movapd	xmm10, xmm1
	mulsd	xmm10, xmm5
	addsd	xmm10, xmm11
	movsd	qword ptr [rdi + 32], xmm10
	movapd	xmm10, xmm2
	mulsd	xmm10, xmm8
	movapd	xmm11, xmm0
	mulsd	xmm11, xmm7
	subsd	xmm10, xmm11
	mulsd	xmm5, xmm6
	addsd	xmm5, xmm10
	movsd	xmm10, qword ptr [rsi + 56]     # xmm10 = mem[0],zero
	movapd	xmm11, xmm1
	mulsd	xmm11, xmm10
	subsd	xmm5, xmm11
	movsd	qword ptr [rdi + 40], xmm5
	mulsd	xmm8, xmm1
	movapd	xmm5, xmm0
	mulsd	xmm5, xmm9
	subsd	xmm8, xmm5
	mulsd	xmm4, xmm6
	subsd	xmm8, xmm4
	movapd	xmm4, xmm2
	mulsd	xmm4, xmm10
	addsd	xmm4, xmm8
	movsd	qword ptr [rdi + 48], xmm4
	mulsd	xmm1, xmm7
	mulsd	xmm2, xmm9
	subsd	xmm1, xmm2
	mulsd	xmm3, xmm6
	addsd	xmm3, xmm1
	mulsd	xmm0, xmm10
	subsd	xmm3, xmm0
	movsd	qword ptr [rdi + 56], xmm3
	ret
.Lfunc_end5:
	.size	_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end5-_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z13written_pointd
.LCPI6_0:
	.quad	0x4000000000000000              # double 2
	.quad	0x4008000000000000              # double 3
	.text
	.globl	_Z13written_pointd
	.prefalign	4, .Lfunc_end6, nop
	.type	_Z13written_pointd,@function
_Z13written_pointd:                     # @_Z13written_pointd
# %bb.0:
	mov	rax, rdi
	movsd	qword ptr [rdi], xmm0
	unpcklpd	xmm0, xmm0                      # xmm0 = xmm0[0,0]
	mulpd	xmm0, xmmword ptr [rip + .LCPI6_0]
	movupd	xmmword ptr [rdi + 8], xmm0
	movabs	rcx, 4607182418800017408
	mov	qword ptr [rdi + 24], rcx
	ret
.Lfunc_end6:
	.size	_Z13written_pointd, .Lfunc_end6-_Z13written_pointd
                                        # -- End function
	.ident	"clang version 23.1.4"
	.section	".note.GNU-stack","",@progbits
	.addrsig
