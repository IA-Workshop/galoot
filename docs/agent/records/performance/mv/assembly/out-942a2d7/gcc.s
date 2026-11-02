	.file	"kernels.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	"_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_"
	.type	"_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_", @function
"_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_":
.LFB1949:
	.cfi_startproc
	movupd	xmm0, XMMWORD PTR [rsi]
	movupd	xmm2, XMMWORD PTR [rdx]
	mov	rax, rdi
	movupd	xmm5, XMMWORD PTR 16[rsi]
	movupd	xmm1, XMMWORD PTR 16[rdx]
	movsd	xmm7, QWORD PTR 8[rsi]
	movapd	xmm4, xmm2
	movsd	xmm3, QWORD PTR [rdx]
	unpckhpd	xmm2, xmm2
	unpcklpd	xmm5, xmm0
	movsd	xmm4, xmm1
	movsd	xmm6, QWORD PTR 16[rsi]
	mulpd	xmm4, xmm5
	movsd	xmm5, QWORD PTR [rsi]
	movapd	xmm8, xmm7
	mulsd	xmm8, xmm2
	mulsd	xmm5, xmm3
	mulsd	xmm7, xmm3
	addsd	xmm5, xmm8
	movapd	xmm8, xmm4
	unpcklpd	xmm5, xmm7
	movsd	xmm7, QWORD PTR 24[rsi]
	addpd	xmm8, xmm5
	subpd	xmm4, xmm5
	movapd	xmm5, xmm3
	mulsd	xmm5, xmm6
	mulsd	xmm3, xmm7
	movsd	xmm4, xmm8
	movups	XMMWORD PTR [rdi], xmm4
	movapd	xmm4, xmm0
	unpcklpd	xmm4, xmm0
	unpckhpd	xmm0, xmm0
	mulpd	xmm4, xmm1
	mulpd	xmm0, xmm1
	unpcklpd	xmm5, xmm3
	movapd	xmm3, xmm4
	subpd	xmm3, xmm5
	movups	XMMWORD PTR 16[rdi], xmm3
	movapd	xmm3, xmm6
	mulsd	xmm3, xmm2
	mulsd	xmm2, xmm7
	unpcklpd	xmm3, xmm2
	movapd	xmm2, xmm1
	subpd	xmm0, xmm3
	mulsd	xmm1, xmm7
	unpckhpd	xmm2, xmm2
	movups	XMMWORD PTR 32[rdi], xmm0
	movapd	xmm0, xmm2
	mulsd	xmm0, xmm6
	subsd	xmm0, xmm1
	movsd	QWORD PTR 48[rdi], xmm0
	ret
	.cfi_endproc
.LFE1949:
	.size	"_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_", .-"_Z22library_point_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_"
	.p2align 4
	.globl	"_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_"
	.type	"_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_", @function
"_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_":
.LFB1957:
	.cfi_startproc
	movupd	xmm4, XMMWORD PTR [rsi]
	movupd	xmm3, XMMWORD PTR [rdx]
	mov	rax, rdi
	movupd	xmm1, XMMWORD PTR 16[rsi]
	movupd	xmm0, XMMWORD PTR 16[rdx]
	movapd	xmm2, xmm4
	movapd	xmm6, xmm3
	movapd	xmm7, xmm4
	movapd	xmm5, xmm3
	unpcklpd	xmm2, xmm4
	unpcklpd	xmm6, xmm3
	shufpd	xmm7, xmm1, 1
	shufpd	xmm5, xmm0, 1
	mulpd	xmm6, xmm7
	mulpd	xmm2, xmm5
	subpd	xmm2, xmm6
	movups	XMMWORD PTR [rdi], xmm2
	movapd	xmm2, xmm0
	shufpd	xmm2, xmm0, 1
	mulpd	xmm2, xmm4
	movapd	xmm4, xmm1
	unpckhpd	xmm0, xmm0
	shufpd	xmm4, xmm1, 1
	mulpd	xmm0, xmm7
	unpckhpd	xmm1, xmm1
	mulpd	xmm3, xmm4
	mulpd	xmm1, xmm5
	subpd	xmm2, xmm3
	subpd	xmm0, xmm1
	movups	XMMWORD PTR 16[rdi], xmm2
	movups	XMMWORD PTR 32[rdi], xmm0
	ret
	.cfi_endproc
.LFE1957:
	.size	"_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_", .-"_Z25library_point_wedge_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEESB_"
	.p2align 4
	.globl	"_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE"
	.type	"_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE", @function
"_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE":
.LFB1958:
	.cfi_startproc
	movsd	xmm2, QWORD PTR [rdx]
	movsd	xmm3, QWORD PTR [rsi]
	mov	rax, rdi
	movsd	xmm4, QWORD PTR 24[rdx]
	movsd	xmm8, QWORD PTR 8[rsi]
	movapd	xmm5, xmm3
	movapd	xmm6, xmm2
	movsd	xmm0, QWORD PTR 8[rdx]
	movsd	xmm7, QWORD PTR 16[rsi]
	mulsd	xmm6, xmm8
	movsd	xmm1, QWORD PTR 16[rdx]
	movsd	xmm10, QWORD PTR 24[rsi]
	mulsd	xmm5, xmm4
	movsd	xmm11, QWORD PTR 32[rsi]
	movsd	xmm9, QWORD PTR 40[rsi]
	movapd	xmm12, xmm1
	mulsd	xmm12, xmm11
	addsd	xmm5, xmm6
	movapd	xmm6, xmm0
	mulsd	xmm6, xmm7
	addsd	xmm5, xmm6
	movapd	xmm6, xmm1
	mulsd	xmm6, xmm10
	addsd	xmm5, xmm6
	movapd	xmm6, xmm0
	mulsd	xmm6, xmm11
	movsd	QWORD PTR 24[rdi], xmm5
	movapd	xmm5, xmm3
	mulsd	xmm5, xmm1
	subsd	xmm5, xmm6
	movapd	xmm6, xmm2
	mulsd	xmm6, xmm9
	addsd	xmm5, xmm6
	movapd	xmm6, xmm3
	mulsd	xmm6, xmm0
	mulsd	xmm3, xmm2
	movsd	QWORD PTR 16[rdi], xmm5
	movsd	xmm5, QWORD PTR 48[rsi]
	addsd	xmm6, xmm12
	movapd	xmm12, xmm2
	mulsd	xmm12, xmm5
	subsd	xmm6, xmm12
	movapd	xmm12, xmm0
	mulsd	xmm12, xmm10
	mulsd	xmm10, xmm2
	movsd	QWORD PTR 8[rdi], xmm6
	movapd	xmm6, xmm1
	mulsd	xmm6, xmm9
	subsd	xmm3, xmm6
	movapd	xmm6, xmm0
	mulsd	xmm6, xmm5
	addsd	xmm3, xmm6
	movapd	xmm6, xmm0
	mulsd	xmm6, xmm9
	mulsd	xmm9, xmm4
	movsd	QWORD PTR [rdi], xmm3
	movapd	xmm3, xmm2
	mulsd	xmm3, xmm11
	mulsd	xmm11, xmm4
	mulsd	xmm4, xmm5
	addsd	xmm3, xmm6
	movapd	xmm6, xmm1
	mulsd	xmm6, xmm5
	addsd	xmm3, xmm6
	movsd	xmm6, QWORD PTR 56[rsi]
	movsd	QWORD PTR 32[rdi], xmm3
	movapd	xmm3, xmm1
	mulsd	xmm3, xmm7
	subsd	xmm3, xmm12
	addsd	xmm3, xmm11
	movapd	xmm11, xmm2
	mulsd	xmm11, xmm6
	mulsd	xmm2, xmm7
	subsd	xmm3, xmm11
	movsd	QWORD PTR 56[rdi], xmm3
	movapd	xmm3, xmm1
	mulsd	xmm3, xmm8
	mulsd	xmm1, xmm6
	subsd	xmm3, xmm10
	subsd	xmm3, xmm9
	movapd	xmm9, xmm0
	mulsd	xmm0, xmm8
	mulsd	xmm9, xmm6
	subsd	xmm0, xmm2
	addsd	xmm3, xmm9
	addsd	xmm0, xmm4
	movsd	QWORD PTR 48[rdi], xmm3
	subsd	xmm0, xmm1
	movsd	QWORD PTR 40[rdi], xmm0
	ret
	.cfi_endproc
.LFE1958:
	.size	"_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE", .-"_Z22library_motor_gp_pointRKN6galoot4core2mvI4rga3XtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_S6_tlS6_Lj0ELi1EEtlS6_Lj9ELin1EEtlS6_Lj10ELin1EEtlS6_Lj12ELin1EEtlS6_Lj6ELi1EEtlS6_Lj5ELin1EEtlS6_Lj3ELi1EEtlS6_Lj15ELi1EEEEEEdEERKNS1_IS2_XtlNS3_ILm4EEEtlS5_IS6_Lm4EEtlA4_S6_tlS6_Lj1ELi1EEtlS6_Lj2ELi1EEtlS6_Lj4ELi1EEtlS6_Lj8ELi1EEEEEEdEE"
	.p2align 4
	.globl	"_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_"
	.type	"_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_", @function
"_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_":
.LFB1959:
	.cfi_startproc
	movupd	xmm0, XMMWORD PTR [rsi]
	movupd	xmm4, XMMWORD PTR 16[rsi]
	mov	rax, rdi
	movupd	xmm1, XMMWORD PTR 16[rdx]
	movsd	xmm7, QWORD PTR 8[rsi]
	unpcklpd	xmm4, xmm0
	movsd	xmm2, QWORD PTR [rdx]
	movsd	xmm5, QWORD PTR 8[rdx]
	movapd	xmm3, xmm1
	movhps	xmm3, QWORD PTR 8[rdx]
	movapd	xmm8, xmm7
	movsd	xmm6, QWORD PTR 16[rsi]
	mulpd	xmm3, xmm4
	movsd	xmm4, QWORD PTR [rsi]
	mulsd	xmm8, xmm5
	mulsd	xmm4, xmm2
	mulsd	xmm7, xmm2
	addsd	xmm4, xmm8
	movapd	xmm8, xmm3
	unpcklpd	xmm4, xmm7
	movsd	xmm7, QWORD PTR 24[rsi]
	addpd	xmm8, xmm4
	subpd	xmm3, xmm4
	movapd	xmm4, xmm2
	mulsd	xmm4, xmm6
	mulsd	xmm2, xmm7
	movsd	xmm3, xmm8
	movups	XMMWORD PTR [rdi], xmm3
	movapd	xmm3, xmm0
	unpcklpd	xmm3, xmm0
	unpckhpd	xmm0, xmm0
	mulpd	xmm3, xmm1
	mulpd	xmm0, xmm1
	unpcklpd	xmm4, xmm2
	movapd	xmm2, xmm3
	subpd	xmm2, xmm4
	movups	XMMWORD PTR 16[rdi], xmm2
	movapd	xmm2, xmm5
	mulsd	xmm2, xmm6
	mulsd	xmm5, xmm7
	unpcklpd	xmm2, xmm5
	movapd	xmm5, xmm1
	subpd	xmm0, xmm2
	mulsd	xmm1, xmm7
	unpckhpd	xmm5, xmm5
	movups	XMMWORD PTR 32[rdi], xmm0
	movapd	xmm0, xmm5
	mulsd	xmm0, xmm6
	subsd	xmm0, xmm1
	movsd	QWORD PTR 48[rdi], xmm0
	ret
	.cfi_endproc
.LFE1959:
	.size	"_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_", .-"_Z19hand_point_gp_pointRKSt5arrayIdLm4EES2_"
	.p2align 4
	.globl	"_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_"
	.type	"_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_", @function
"_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_":
.LFB1960:
	.cfi_startproc
	movupd	xmm4, XMMWORD PTR [rsi]
	movupd	xmm3, XMMWORD PTR [rdx]
	mov	rax, rdi
	movupd	xmm1, XMMWORD PTR 16[rsi]
	movupd	xmm0, XMMWORD PTR 16[rdx]
	movapd	xmm2, xmm4
	movapd	xmm6, xmm3
	movapd	xmm7, xmm4
	movapd	xmm5, xmm3
	unpcklpd	xmm2, xmm4
	unpcklpd	xmm6, xmm3
	shufpd	xmm7, xmm1, 1
	shufpd	xmm5, xmm0, 1
	mulpd	xmm6, xmm7
	mulpd	xmm2, xmm5
	subpd	xmm2, xmm6
	movups	XMMWORD PTR [rdi], xmm2
	movapd	xmm2, xmm0
	shufpd	xmm2, xmm0, 1
	mulpd	xmm2, xmm4
	movapd	xmm4, xmm1
	unpckhpd	xmm0, xmm0
	shufpd	xmm4, xmm1, 1
	mulpd	xmm0, xmm7
	unpckhpd	xmm1, xmm1
	mulpd	xmm3, xmm4
	mulpd	xmm1, xmm5
	subpd	xmm2, xmm3
	subpd	xmm0, xmm1
	movups	XMMWORD PTR 16[rdi], xmm2
	movups	XMMWORD PTR 32[rdi], xmm0
	ret
	.cfi_endproc
.LFE1960:
	.size	"_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_", .-"_Z22hand_point_wedge_pointRKSt5arrayIdLm4EES2_"
	.p2align 4
	.globl	"_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE"
	.type	"_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE", @function
"_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE":
.LFB1961:
	.cfi_startproc
	movsd	xmm1, QWORD PTR [rsi]
	movsd	xmm9, QWORD PTR 40[rsi]
	mov	rax, rdi
	movsd	xmm3, QWORD PTR [rdx]
	movsd	xmm0, QWORD PTR 16[rdx]
	movapd	xmm2, xmm1
	movapd	xmm5, xmm9
	movsd	xmm4, QWORD PTR 8[rdx]
	movsd	xmm10, QWORD PTR 48[rsi]
	mulsd	xmm5, xmm0
	movapd	xmm6, xmm0
	movapd	xmm11, xmm3
	movsd	xmm8, QWORD PTR 16[rsi]
	mulsd	xmm2, xmm3
	movsd	xmm7, QWORD PTR 24[rsi]
	movapd	xmm12, xmm3
	mulsd	xmm12, xmm8
	subsd	xmm2, xmm5
	movapd	xmm5, xmm10
	mulsd	xmm5, xmm4
	addsd	xmm2, xmm5
	movsd	xmm5, QWORD PTR 32[rsi]
	mulsd	xmm6, xmm5
	movsd	QWORD PTR [rdi], xmm2
	movapd	xmm2, xmm1
	mulsd	xmm2, xmm4
	addsd	xmm2, xmm6
	movapd	xmm6, xmm3
	mulsd	xmm6, xmm10
	subsd	xmm2, xmm6
	movapd	xmm6, xmm4
	mulsd	xmm6, xmm5
	movsd	QWORD PTR 8[rdi], xmm2
	movapd	xmm2, xmm1
	mulsd	xmm2, xmm0
	subsd	xmm2, xmm6
	movapd	xmm6, xmm3
	mulsd	xmm6, xmm9
	addsd	xmm2, xmm6
	movsd	xmm6, QWORD PTR 24[rdx]
	mulsd	xmm1, xmm6
	movsd	QWORD PTR 16[rdi], xmm2
	movsd	xmm2, QWORD PTR 8[rsi]
	mulsd	xmm11, xmm2
	addsd	xmm1, xmm11
	movapd	xmm11, xmm4
	mulsd	xmm11, xmm8
	addsd	xmm1, xmm11
	movapd	xmm11, xmm0
	mulsd	xmm11, xmm7
	addsd	xmm1, xmm11
	movapd	xmm11, xmm9
	mulsd	xmm11, xmm4
	mulsd	xmm9, xmm6
	movsd	QWORD PTR 24[rdi], xmm1
	movapd	xmm1, xmm3
	mulsd	xmm1, xmm5
	mulsd	xmm5, xmm6
	addsd	xmm1, xmm11
	movapd	xmm11, xmm0
	mulsd	xmm11, xmm10
	mulsd	xmm10, xmm6
	addsd	xmm1, xmm11
	movapd	xmm11, xmm4
	mulsd	xmm11, xmm2
	mulsd	xmm2, xmm0
	movsd	QWORD PTR 32[rdi], xmm1
	movsd	xmm1, QWORD PTR 56[rsi]
	subsd	xmm11, xmm12
	addsd	xmm11, xmm10
	movapd	xmm10, xmm0
	mulsd	xmm10, xmm1
	mulsd	xmm0, xmm8
	subsd	xmm11, xmm10
	movapd	xmm10, xmm3
	mulsd	xmm10, xmm7
	mulsd	xmm3, xmm1
	movsd	QWORD PTR 40[rdi], xmm11
	subsd	xmm2, xmm10
	subsd	xmm2, xmm9
	movapd	xmm9, xmm4
	mulsd	xmm4, xmm7
	mulsd	xmm9, xmm1
	subsd	xmm0, xmm4
	addsd	xmm2, xmm9
	addsd	xmm0, xmm5
	movsd	QWORD PTR 48[rdi], xmm2
	subsd	xmm0, xmm3
	movsd	QWORD PTR 56[rdi], xmm0
	ret
	.cfi_endproc
.LFE1961:
	.size	"_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE", .-"_Z19hand_motor_gp_pointRKSt5arrayIdLm8EERKS_IdLm4EE"
	.p2align 4
	.globl	"_Z13written_pointd"
	.type	"_Z13written_pointd", @function
"_Z13written_pointd":
.LFB1962:
	.cfi_startproc
	movapd	xmm1, xmm0
	movsd	QWORD PTR [rdi], xmm0
	mov	rdx, QWORD PTR .LC1[rip]
	mov	rax, rdi
	addsd	xmm1, xmm0
	mulsd	xmm0, QWORD PTR .LC0[rip]
	mov	QWORD PTR 24[rdi], rdx
	movsd	QWORD PTR 8[rdi], xmm1
	movsd	QWORD PTR 16[rdi], xmm0
	ret
	.cfi_endproc
.LFE1962:
	.size	"_Z13written_pointd", .-"_Z13written_pointd"
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	0
	.long	1074266112
	.align 8
.LC1:
	.long	0
	.long	1072693248
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
