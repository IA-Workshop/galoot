	.att_syntax
	.file	"probe.cpp"
	.text
	.globl	_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end0, nop
	.type	_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$112, %rsp
	.cfi_def_cfa_offset 128
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%fs:40, %rax
	movq	%rax, 104(%rsp)
	movups	(%rdx), %xmm0
	movups	16(%rdx), %xmm1
	movaps	%xmm1, 80(%rsp)
	movaps	%xmm0, 64(%rsp)
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	32(%rsi), %xmm2
	movups	48(%rsi), %xmm3
	movaps	%xmm3, 48(%rsp)
	movaps	%xmm2, 32(%rsp)
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, (%rsp)
	leaq	64(%rsp), %rsi
	movq	%rsp, %rdx
	callq	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	movq	%fs:40, %rax
	cmpq	104(%rsp), %rax
	jne	.LBB0_2
# %bb.1:
	movq	%rbx, %rax
	addq	$112, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB0_2:
	.cfi_def_cfa_offset 128
	callq	__stack_chk_fail@PLT
.Lfunc_end0:
	.size	_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end0-_Z12point_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end1, nop
	.type	_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm6                   # xmm6 = mem[0],zero
	movsd	8(%rdx), %xmm7                  # xmm7 = mem[0],zero
	movsd	(%rsi), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rsi), %xmm8                  # xmm8 = mem[0],zero
	movapd	%xmm1, %xmm3
	mulsd	%xmm6, %xmm3
	movapd	%xmm1, %xmm0
	mulsd	%xmm7, %xmm0
	movsd	16(%rdx), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm1, %xmm4
	mulsd	%xmm9, %xmm4
	movsd	24(%rdx), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm1, %xmm2
	mulsd	%xmm12, %xmm2
	movsd	%xmm2, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm6, %xmm5
	movapd	%xmm8, %xmm2
	mulsd	%xmm8, %xmm5
	subsd	%xmm5, %xmm0
	movapd	%xmm7, %xmm8
	mulsd	%xmm2, %xmm8
	addsd	%xmm3, %xmm8
	movapd	%xmm9, %xmm10
	mulsd	%xmm2, %xmm10
	movsd	%xmm12, -16(%rsp)               # 8-byte Spill
	movsd	16(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm6, %xmm13
	mulsd	%xmm3, %xmm13
	subsd	%xmm4, %xmm13
	movapd	%xmm7, %xmm4
	mulsd	%xmm3, %xmm4
	subsd	%xmm4, %xmm10
	movapd	%xmm9, %xmm4
	mulsd	%xmm3, %xmm4
	addsd	%xmm8, %xmm4
	movapd	%xmm12, %xmm8
	movsd	24(%rsi), %xmm11                # xmm11 = mem[0],zero
	mulsd	%xmm11, %xmm6
	addsd	%xmm10, %xmm6
	mulsd	%xmm11, %xmm7
	addsd	%xmm13, %xmm7
	mulsd	%xmm11, %xmm9
	addsd	%xmm0, %xmm9
	movsd	32(%rsi), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm12, %xmm13
	mulsd	%xmm10, %xmm13
	addsd	%xmm6, %xmm13
	movsd	40(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm12, %xmm15
	mulsd	%xmm6, %xmm15
	addsd	%xmm7, %xmm15
	movsd	48(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm12, %xmm14
	mulsd	%xmm7, %xmm14
	addsd	%xmm9, %xmm14
	movapd	%xmm12, %xmm0
	movsd	56(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movsd	%xmm5, -24(%rsp)                # 8-byte Spill
	mulsd	%xmm5, %xmm12
	subsd	%xmm12, %xmm4
	movapd	%xmm2, %xmm12
	movapd	%xmm2, %xmm9
	mulsd	%xmm13, %xmm12
	movapd	%xmm1, %xmm2
	mulsd	%xmm15, %xmm1
	subsd	%xmm12, %xmm1
	movapd	%xmm3, %xmm12
	mulsd	%xmm13, %xmm12
	mulsd	%xmm11, %xmm13
	movapd	%xmm3, %xmm5
	mulsd	%xmm15, %xmm5
	subsd	%xmm5, %xmm13
	mulsd	%xmm11, %xmm15
	addsd	%xmm12, %xmm15
	movapd	%xmm2, %xmm5
	mulsd	%xmm14, %xmm5
	subsd	%xmm5, %xmm15
	movapd	%xmm9, %xmm5
	mulsd	%xmm14, %xmm5
	addsd	%xmm13, %xmm5
	mulsd	%xmm11, %xmm14
	addsd	%xmm1, %xmm14
	mulsd	%xmm11, %xmm0
	movapd	%xmm10, %xmm1
	mulsd	%xmm0, %xmm1
	addsd	%xmm5, %xmm1
	movapd	%xmm0, %xmm5
	mulsd	%xmm6, %xmm5
	addsd	%xmm15, %xmm5
	mulsd	%xmm0, %xmm11
	mulsd	%xmm7, %xmm0
	addsd	%xmm14, %xmm0
	movapd	%xmm2, %xmm13
	movapd	%xmm2, %xmm12
	mulsd	%xmm4, %xmm12
	addsd	%xmm1, %xmm12
	movapd	%xmm9, %xmm1
	movapd	%xmm9, %xmm2
	mulsd	%xmm4, %xmm1
	addsd	%xmm5, %xmm1
	mulsd	%xmm3, %xmm4
	addsd	%xmm0, %xmm4
	mulsd	%xmm3, %xmm8
	mulsd	%xmm8, %xmm3
	addsd	%xmm11, %xmm3
	movapd	%xmm8, %xmm0
	mulsd	%xmm10, %xmm0
	addsd	%xmm1, %xmm0
	movapd	%xmm8, %xmm1
	mulsd	%xmm6, %xmm1
	subsd	%xmm1, %xmm12
	movsd	-24(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	mulsd	%xmm9, %xmm8
	subsd	%xmm8, %xmm4
	movapd	%xmm2, %xmm1
	movsd	-16(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	mulsd	%xmm1, %xmm2
	mulsd	%xmm2, %xmm1
	addsd	%xmm3, %xmm1
	movapd	%xmm1, %xmm5
	mulsd	%xmm2, %xmm10
	subsd	%xmm10, %xmm4
	movapd	%xmm2, %xmm1
	mulsd	%xmm7, %xmm1
	addsd	%xmm12, %xmm1
	mulsd	%xmm9, %xmm2
	subsd	%xmm2, %xmm0
	movapd	%xmm13, %xmm2
	movsd	-8(%rsp), %xmm3                 # 8-byte Reload
                                        # xmm3 = mem[0],zero
	mulsd	%xmm3, %xmm2
	addsd	%xmm5, %xmm2
	mulsd	%xmm3, %xmm6
	addsd	%xmm4, %xmm6
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm0
	mulsd	%xmm9, %xmm3
	subsd	%xmm3, %xmm1
	movsd	%xmm1, (%rdi)
	movsd	%xmm0, 8(%rdi)
	movsd	%xmm6, 16(%rdi)
	movsd	%xmm2, 24(%rdi)
	retq
.Lfunc_end1:
	.size	_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end1-_Z7point_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end2, nop
	.type	_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm5                   # xmm5 = mem[0],zero
	movsd	8(%rsi), %xmm4                  # xmm4 = mem[0],zero
	movsd	16(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movsd	24(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movapd	%xmm5, %xmm0
	mulsd	%xmm4, %xmm0
	movapd	%xmm9, %xmm12
	mulsd	%xmm2, %xmm12
	movapd	%xmm0, %xmm13
	subsd	%xmm12, %xmm13
	movapd	%xmm5, %xmm1
	mulsd	%xmm9, %xmm1
	movapd	%xmm4, %xmm8
	mulsd	%xmm2, %xmm8
	movapd	%xmm1, %xmm6
	addsd	%xmm8, %xmm6
	movsd	56(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm5, %xmm3
	mulsd	%xmm11, %xmm3
	movsd	%xmm11, -24(%rsp)               # 8-byte Spill
	movsd	48(%rsi), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm4, %xmm7
	mulsd	%xmm10, %xmm7
	movsd	%xmm10, -16(%rsp)               # 8-byte Spill
	subsd	%xmm3, %xmm7
	movsd	40(%rsi), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm9, %xmm3
	mulsd	%xmm14, %xmm3
	movsd	%xmm14, -32(%rsp)               # 8-byte Spill
	subsd	%xmm3, %xmm7
	movsd	32(%rsi), %xmm15                # xmm15 = mem[0],zero
	movapd	%xmm2, %xmm3
	mulsd	%xmm15, %xmm3
	addsd	%xmm7, %xmm3
	addsd	%xmm0, %xmm12
	movapd	%xmm5, %xmm0
	mulsd	%xmm10, %xmm0
	movapd	%xmm4, %xmm7
	mulsd	%xmm11, %xmm7
	addsd	%xmm0, %xmm7
	movapd	%xmm9, %xmm0
	mulsd	%xmm15, %xmm0
	subsd	%xmm7, %xmm0
	movapd	%xmm2, %xmm7
	mulsd	%xmm14, %xmm7
	addsd	%xmm0, %xmm7
	movapd	%xmm5, %xmm0
	mulsd	%xmm2, %xmm0
	subsd	%xmm8, %xmm1
	movsd	%xmm1, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm4, %xmm11
	mulsd	%xmm9, %xmm11
	movapd	%xmm11, %xmm14
	subsd	%xmm0, %xmm14
	addsd	%xmm0, %xmm11
	movapd	%xmm5, %xmm0
	mulsd	%xmm5, %xmm0
	movapd	%xmm4, %xmm8
	mulsd	%xmm4, %xmm8
	movapd	%xmm0, %xmm1
	subsd	%xmm8, %xmm1
	movapd	%xmm8, %xmm10
	subsd	%xmm0, %xmm10
	addsd	%xmm0, %xmm8
	mulsd	-32(%rsp), %xmm5                # 8-byte Folded Reload
	mulsd	%xmm15, %xmm4
	subsd	%xmm4, %xmm5
	movapd	%xmm9, %xmm0
	mulsd	-24(%rsp), %xmm9                # 8-byte Folded Reload
	subsd	%xmm9, %xmm5
	movapd	%xmm2, %xmm9
	mulsd	-16(%rsp), %xmm2                # 8-byte Folded Reload
	mulsd	%xmm0, %xmm0
	mulsd	%xmm9, %xmm9
	subsd	%xmm0, %xmm1
	addsd	%xmm9, %xmm1
	addsd	%xmm13, %xmm13
	addsd	%xmm6, %xmm6
	addsd	%xmm3, %xmm3
	subsd	%xmm0, %xmm10
	addsd	%xmm9, %xmm10
	addsd	%xmm5, %xmm2
	movapd	%xmm0, %xmm4
	subsd	%xmm8, %xmm4
	addsd	%xmm9, %xmm4
	addsd	%xmm0, %xmm8
	addsd	%xmm9, %xmm8
	movsd	(%rdx), %xmm9                   # xmm9 = mem[0],zero
	movsd	8(%rdx), %xmm5                  # xmm5 = mem[0],zero
	mulsd	%xmm9, %xmm1
	mulsd	%xmm5, %xmm13
	addsd	%xmm1, %xmm13
	movsd	16(%rdx), %xmm15                # xmm15 = mem[0],zero
	mulsd	%xmm15, %xmm6
	addsd	%xmm13, %xmm6
	movsd	24(%rdx), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm3
	addsd	%xmm6, %xmm3
	addsd	%xmm12, %xmm12
	mulsd	%xmm9, %xmm12
	mulsd	%xmm5, %xmm10
	addsd	%xmm12, %xmm10
	addsd	%xmm14, %xmm14
	mulsd	%xmm15, %xmm14
	addsd	%xmm10, %xmm14
	addsd	%xmm7, %xmm7
	mulsd	%xmm0, %xmm7
	addsd	%xmm14, %xmm7
	movsd	-8(%rsp), %xmm1                 # 8-byte Reload
                                        # xmm1 = mem[0],zero
	addsd	%xmm1, %xmm1
	mulsd	%xmm9, %xmm1
	addsd	%xmm11, %xmm11
	mulsd	%xmm5, %xmm11
	addsd	%xmm1, %xmm11
	mulsd	%xmm15, %xmm4
	addsd	%xmm11, %xmm4
	addsd	%xmm2, %xmm2
	mulsd	%xmm0, %xmm2
	addsd	%xmm4, %xmm2
	mulsd	%xmm0, %xmm8
	movsd	%xmm3, (%rdi)
	movsd	%xmm7, 8(%rdi)
	movsd	%xmm2, 16(%rdi)
	movsd	%xmm8, 24(%rdi)
	retq
.Lfunc_end2:
	.size	_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end2-_Z7point_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end3, nop
	.type	_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm6                   # xmm6 = mem[0],zero
	movsd	8(%rsi), %xmm0                  # xmm0 = mem[0],zero
	movsd	%xmm6, -16(%rsp)                # 8-byte Spill
	movapd	%xmm6, %xmm9
	mulsd	%xmm0, %xmm9
	movsd	16(%rsi), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm6, %xmm1
	mulsd	%xmm12, %xmm1
	movsd	%xmm1, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm6, %xmm7
	movsd	40(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm6, %xmm10
	mulsd	%xmm3, %xmm10
	movsd	48(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm6, %xmm1
	mulsd	%xmm11, %xmm1
	movsd	56(%rsi), %xmm14                # xmm14 = mem[0],zero
	mulsd	%xmm14, %xmm6
	movapd	%xmm0, %xmm2
	movapd	%xmm0, %xmm15
	mulsd	%xmm12, %xmm15
	movsd	32(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm0, %xmm4
	mulsd	%xmm5, %xmm4
	subsd	%xmm4, %xmm10
	movapd	%xmm0, %xmm13
	mulsd	%xmm11, %xmm13
	subsd	%xmm6, %xmm13
	movapd	%xmm12, %xmm4
	mulsd	%xmm5, %xmm4
	subsd	%xmm1, %xmm4
	movapd	%xmm0, %xmm8
	mulsd	%xmm14, %xmm0
	subsd	%xmm0, %xmm4
	movapd	%xmm12, %xmm0
	mulsd	%xmm3, %xmm0
	subsd	%xmm0, %xmm13
	movapd	%xmm12, %xmm1
	movapd	%xmm12, %xmm6
	mulsd	%xmm14, %xmm12
	movsd	-16(%rsp), %xmm14               # 8-byte Reload
                                        # xmm14 = mem[0],zero
	mulsd	%xmm14, %xmm14
	subsd	%xmm12, %xmm10
	movsd	24(%rsi), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm7
	movsd	%xmm7, -16(%rsp)                # 8-byte Spill
	mulsd	%xmm2, %xmm2
	mulsd	%xmm0, %xmm8
	mulsd	%xmm1, %xmm1
	mulsd	%xmm0, %xmm6
	mulsd	%xmm0, %xmm5
	addsd	%xmm13, %xmm5
	movapd	%xmm0, %xmm12
	mulsd	%xmm0, %xmm3
	addsd	%xmm4, %xmm3
	mulsd	%xmm11, %xmm0
	addsd	%xmm10, %xmm0
	movapd	%xmm14, %xmm7
	movapd	%xmm14, %xmm13
	subsd	%xmm2, %xmm13
	movapd	%xmm9, %xmm14
	subsd	%xmm6, %xmm14
	movsd	-8(%rsp), %xmm4                 # 8-byte Reload
                                        # xmm4 = mem[0],zero
	movapd	%xmm4, %xmm11
	addsd	%xmm8, %xmm11
	addsd	%xmm9, %xmm6
	movapd	%xmm2, %xmm9
	subsd	%xmm7, %xmm9
	subsd	%xmm8, %xmm4
	movapd	%xmm15, %xmm10
	movsd	-16(%rsp), %xmm8                # 8-byte Reload
                                        # xmm8 = mem[0],zero
	subsd	%xmm8, %xmm10
	addsd	%xmm8, %xmm15
	movapd	%xmm1, %xmm8
	subsd	%xmm7, %xmm8
	subsd	%xmm2, %xmm8
	addsd	%xmm7, %xmm2
	mulsd	%xmm12, %xmm12
	subsd	%xmm1, %xmm13
	addsd	%xmm12, %xmm13
	subsd	%xmm1, %xmm9
	addsd	%xmm12, %xmm9
	addsd	%xmm12, %xmm8
	addsd	%xmm1, %xmm2
	addsd	%xmm12, %xmm2
	addsd	%xmm14, %xmm14
	addsd	%xmm11, %xmm11
	addsd	%xmm5, %xmm5
	movsd	(%rdx), %xmm7                   # xmm7 = mem[0],zero
	movsd	8(%rdx), %xmm12                 # xmm12 = mem[0],zero
	mulsd	%xmm7, %xmm13
	mulsd	%xmm12, %xmm14
	addsd	%xmm13, %xmm14
	movsd	16(%rdx), %xmm13                # xmm13 = mem[0],zero
	mulsd	%xmm13, %xmm11
	addsd	%xmm14, %xmm11
	movsd	24(%rdx), %xmm1                 # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm5
	addsd	%xmm11, %xmm5
	movsd	%xmm5, (%rdi)
	addsd	%xmm6, %xmm6
	mulsd	%xmm7, %xmm6
	mulsd	%xmm12, %xmm9
	addsd	%xmm6, %xmm9
	addsd	%xmm10, %xmm10
	mulsd	%xmm13, %xmm10
	addsd	%xmm9, %xmm10
	addsd	%xmm3, %xmm3
	mulsd	%xmm1, %xmm3
	addsd	%xmm10, %xmm3
	movsd	%xmm3, 8(%rdi)
	movapd	%xmm4, %xmm3
	addsd	%xmm4, %xmm3
	mulsd	%xmm7, %xmm3
	addsd	%xmm15, %xmm15
	mulsd	%xmm12, %xmm15
	addsd	%xmm3, %xmm15
	mulsd	%xmm13, %xmm8
	addsd	%xmm15, %xmm8
	addsd	%xmm0, %xmm0
	mulsd	%xmm1, %xmm0
	addsd	%xmm8, %xmm0
	movsd	%xmm0, 16(%rdi)
	mulsd	%xmm1, %xmm2
	movsd	%xmm2, 24(%rdi)
	retq
.Lfunc_end3:
	.size	_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end3-_Z8point_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end4, nop
	.type	_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm10                  # xmm10 = mem[0],zero
	movsd	8(%rsi), %xmm5                  # xmm5 = mem[0],zero
	movapd	%xmm10, %xmm2
	addsd	%xmm10, %xmm2
	movapd	%xmm5, %xmm0
	addsd	%xmm5, %xmm0
	movsd	16(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm9, %xmm6
	addsd	%xmm9, %xmm6
	movsd	16(%rdx), %xmm1                 # xmm1 = mem[0],zero
	movapd	%xmm5, %xmm8
	mulsd	%xmm1, %xmm8
	movsd	(%rdx), %xmm4                   # xmm4 = mem[0],zero
	movsd	8(%rdx), %xmm3                  # xmm3 = mem[0],zero
	movapd	%xmm9, %xmm7
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm8
	mulsd	%xmm4, %xmm9
	movapd	%xmm10, %xmm7
	mulsd	%xmm1, %xmm7
	subsd	%xmm7, %xmm9
	mulsd	%xmm3, %xmm10
	mulsd	%xmm4, %xmm5
	subsd	%xmm5, %xmm10
	movsd	24(%rdx), %xmm5                 # xmm5 = mem[0],zero
	movsd	32(%rsi), %xmm7                 # xmm7 = mem[0],zero
	mulsd	%xmm5, %xmm7
	addsd	%xmm8, %xmm7
	movsd	40(%rsi), %xmm8                 # xmm8 = mem[0],zero
	mulsd	%xmm5, %xmm8
	addsd	%xmm9, %xmm8
	movsd	48(%rsi), %xmm9                 # xmm9 = mem[0],zero
	mulsd	%xmm5, %xmm9
	addsd	%xmm10, %xmm9
	movsd	56(%rsi), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm5, %xmm10
	movapd	%xmm0, %xmm11
	mulsd	%xmm9, %xmm11
	movapd	%xmm6, %xmm12
	movapd	%xmm2, %xmm14
	movapd	%xmm2, %xmm13
	movapd	%xmm0, %xmm15
	mulsd	%xmm10, %xmm2
	mulsd	%xmm10, %xmm0
	mulsd	%xmm6, %xmm10
	mulsd	%xmm8, %xmm6
	subsd	%xmm6, %xmm11
	movsd	24(%rsi), %xmm6                 # xmm6 = mem[0],zero
	addsd	%xmm6, %xmm6
	mulsd	%xmm7, %xmm12
	mulsd	%xmm9, %xmm14
	subsd	%xmm14, %xmm12
	mulsd	%xmm8, %xmm13
	mulsd	%xmm7, %xmm15
	subsd	%xmm15, %xmm13
	mulsd	%xmm6, %xmm7
	addsd	%xmm11, %xmm7
	subsd	%xmm2, %xmm7
	addsd	%xmm4, %xmm7
	mulsd	%xmm6, %xmm8
	addsd	%xmm12, %xmm8
	subsd	%xmm0, %xmm8
	addsd	%xmm3, %xmm8
	mulsd	%xmm6, %xmm9
	addsd	%xmm13, %xmm9
	subsd	%xmm10, %xmm9
	addsd	%xmm1, %xmm9
	movsd	%xmm7, (%rdi)
	movsd	%xmm8, 8(%rdi)
	movsd	%xmm9, 16(%rdi)
	movsd	%xmm5, 24(%rdi)
	retq
.Lfunc_end4:
	.size	_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end4-_Z7point_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13point_b_setupRKSt5arrayIdLm8EE # -- Begin function _Z13point_b_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end5, nop
	.type	_Z13point_b_setupRKSt5arrayIdLm8EE,@function
_Z13point_b_setupRKSt5arrayIdLm8EE:     # @_Z13point_b_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	56(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movsd	%xmm4, -32(%rsp)                # 8-byte Spill
	movsd	48(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movsd	40(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movsd	%xmm3, -40(%rsp)                # 8-byte Spill
	movsd	32(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movsd	24(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movsd	16(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movsd	(%rsi), %xmm6                   # xmm6 = mem[0],zero
	movsd	8(%rsi), %xmm1                  # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm7
	mulsd	%xmm1, %xmm7
	movapd	%xmm6, %xmm11
	mulsd	%xmm6, %xmm11
	movapd	%xmm6, %xmm9
	mulsd	%xmm4, %xmm9
	movapd	%xmm1, %xmm12
	movapd	%xmm1, %xmm4
	mulsd	%xmm0, %xmm12
	movapd	%xmm0, %xmm1
	subsd	%xmm9, %xmm12
	movapd	%xmm8, %xmm9
	mulsd	%xmm3, %xmm9
	subsd	%xmm9, %xmm12
	movapd	%xmm5, %xmm13
	mulsd	%xmm2, %xmm13
	movapd	%xmm4, %xmm0
	mulsd	%xmm5, %xmm0
	movapd	%xmm0, %xmm3
	movsd	%xmm0, -56(%rsp)                # 8-byte Spill
	movapd	%xmm11, %xmm14
	subsd	%xmm7, %xmm14
	movapd	%xmm6, %xmm0
	mulsd	%xmm8, %xmm0
	movsd	%xmm0, -48(%rsp)                # 8-byte Spill
	movapd	%xmm8, %xmm15
	mulsd	%xmm8, %xmm15
	addsd	%xmm12, %xmm13
	movapd	%xmm0, %xmm9
	addsd	%xmm3, %xmm9
	unpcklpd	%xmm13, %xmm9                   # xmm9 = xmm9[0],xmm13[0]
	movapd	%xmm9, -24(%rsp)                # 16-byte Spill
	subsd	%xmm15, %xmm14
	movapd	%xmm7, %xmm13
	subsd	%xmm11, %xmm13
	subsd	%xmm15, %xmm13
	addsd	%xmm7, %xmm11
	movapd	%xmm15, %xmm0
	subsd	%xmm11, %xmm0
	movapd	%xmm0, %xmm7
	addsd	%xmm15, %xmm11
	movapd	%xmm5, %xmm0
	mulsd	%xmm5, %xmm0
	movapd	%xmm6, %xmm10
	movapd	%xmm4, %xmm3
	mulsd	%xmm4, %xmm10
	movapd	%xmm8, %xmm15
	mulsd	%xmm5, %xmm15
	movapd	%xmm10, %xmm4
	subsd	%xmm15, %xmm4
	addsd	%xmm0, %xmm13
	addsd	%xmm0, %xmm7
	movsd	%xmm7, -64(%rsp)                # 8-byte Spill
	addsd	%xmm0, %xmm11
	unpcklpd	%xmm4, %xmm0                    # xmm0 = xmm0[0],xmm4[0]
	unpcklpd	%xmm4, %xmm14                   # xmm14 = xmm14[0],xmm4[0]
	addpd	%xmm0, %xmm14
	addsd	%xmm10, %xmm15
	movapd	%xmm6, %xmm0
	movapd	%xmm1, %xmm12
	mulsd	%xmm1, %xmm0
	movapd	%xmm3, %xmm4
	movapd	%xmm3, %xmm7
	movsd	-32(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	mulsd	%xmm9, %xmm4
	addsd	%xmm0, %xmm4
	movapd	%xmm8, %xmm1
	movapd	%xmm8, %xmm0
	movapd	%xmm2, %xmm3
	mulsd	%xmm2, %xmm0
	subsd	%xmm4, %xmm0
	movapd	%xmm5, %xmm4
	movsd	-40(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm4
	addsd	%xmm0, %xmm4
	movapd	%xmm7, %xmm8
	mulsd	%xmm1, %xmm8
	movapd	%xmm6, %xmm10
	mulsd	%xmm5, %xmm10
	movapd	%xmm8, %xmm0
	subsd	%xmm10, %xmm0
	unpcklpd	%xmm4, %xmm0                    # xmm0 = xmm0[0],xmm4[0]
	movsd	-48(%rsp), %xmm4                # 8-byte Reload
                                        # xmm4 = mem[0],zero
	subsd	-56(%rsp), %xmm4                # 8-byte Folded Reload
	addsd	%xmm8, %xmm10
	mulsd	%xmm2, %xmm6
	mulsd	%xmm3, %xmm7
	subsd	%xmm7, %xmm6
	mulsd	%xmm9, %xmm1
	subsd	%xmm1, %xmm6
	mulsd	%xmm12, %xmm5
	addsd	%xmm6, %xmm5
	movupd	%xmm14, (%rdi)
	movapd	-24(%rsp), %xmm1                # 16-byte Reload
	addpd	%xmm1, %xmm1
	movupd	%xmm1, 16(%rdi)
	addsd	%xmm15, %xmm15
	movsd	%xmm15, 32(%rdi)
	movsd	%xmm13, 40(%rdi)
	addpd	%xmm0, %xmm0
	movupd	%xmm0, 48(%rdi)
	movapd	%xmm4, %xmm0
	addsd	%xmm4, %xmm0
	movsd	%xmm0, 64(%rdi)
	addsd	%xmm10, %xmm10
	movsd	%xmm10, 72(%rdi)
	movsd	-64(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movsd	%xmm0, 80(%rdi)
	addsd	%xmm5, %xmm5
	movsd	%xmm5, 88(%rdi)
	movsd	%xmm11, 96(%rdi)
	retq
.Lfunc_end5:
	.size	_Z13point_b_setupRKSt5arrayIdLm8EE, .Lfunc_end5-_Z13point_b_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE # -- Begin function _Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end6, nop
	.type	_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE,@function
_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE: # @_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	16(%rcx), %xmm6                 # xmm6 = mem[0],zero
	movsd	24(%rcx), %xmm5                 # xmm5 = mem[0],zero
	movsd	80(%rsi), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm6, %xmm0
	movsd	88(%rsi), %xmm1                 # xmm1 = mem[0],zero
	mulsd	%xmm5, %xmm1
	movsd	96(%rsi), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm5, %xmm2
	movupd	(%rcx), %xmm3
	movupd	(%rsi), %xmm7
	movupd	16(%rsi), %xmm4
	movupd	32(%rsi), %xmm8
	movupd	48(%rsi), %xmm9
	movapd	%xmm3, %xmm10
	shufpd	$1, %xmm3, %xmm10               # xmm10 = xmm10[1],xmm3[0]
	movapd	%xmm7, %xmm11
	shufpd	$1, %xmm8, %xmm11               # xmm11 = xmm11[1],xmm8[0]
	mulpd	%xmm10, %xmm11
	movsd	%xmm7, %xmm8                    # xmm8 = xmm7[0],xmm8[1]
	mulpd	%xmm3, %xmm8
	addpd	%xmm11, %xmm8
	unpcklpd	%xmm6, %xmm6                    # xmm6 = xmm6[0,0]
	movapd	%xmm4, %xmm7
	unpcklpd	%xmm9, %xmm7                    # xmm7 = xmm7[0],xmm9[0]
	mulpd	%xmm6, %xmm7
	addpd	%xmm8, %xmm7
	unpcklpd	%xmm5, %xmm5                    # xmm5 = xmm5[0,0]
	unpckhpd	%xmm9, %xmm4                    # xmm4 = xmm4[1],xmm9[1]
	mulpd	%xmm5, %xmm4
	movsd	64(%rsi), %xmm5                 # xmm5 = mem[0],zero
	mulsd	%xmm3, %xmm5
	unpckhpd	%xmm3, %xmm3                    # xmm3 = xmm3[1,1]
	mulsd	72(%rsi), %xmm3
	addpd	%xmm7, %xmm4
	addsd	%xmm5, %xmm3
	addsd	%xmm0, %xmm3
	addsd	%xmm1, %xmm3
	movupd	%xmm4, (%rdi)
	movsd	%xmm3, 16(%rdi)
	movsd	%xmm2, 24(%rdi)
	retq
.Lfunc_end6:
	.size	_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE, .Lfunc_end6-_Z13point_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z14point_bp_setupRKSt5arrayIdLm8EE # -- Begin function _Z14point_bp_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end7, nop
	.type	_Z14point_bp_setupRKSt5arrayIdLm8EE,@function
_Z14point_bp_setupRKSt5arrayIdLm8EE:    # @_Z14point_bp_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	48(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movsd	56(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movsd	40(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movsd	32(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movsd	24(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movsd	16(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movsd	(%rsi), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rsi), %xmm5                  # xmm5 = mem[0],zero
	movapd	%xmm1, %xmm7
	mulsd	%xmm6, %xmm7
	movapd	%xmm1, %xmm12
	mulsd	%xmm0, %xmm12
	movapd	%xmm1, %xmm11
	mulsd	%xmm8, %xmm11
	movapd	%xmm5, %xmm2
	movapd	%xmm5, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm10, %xmm7
	movapd	%xmm5, %xmm10
	mulsd	%xmm0, %xmm10
	subsd	%xmm11, %xmm10
	movapd	%xmm5, %xmm13
	mulsd	%xmm8, %xmm13
	movapd	%xmm4, %xmm11
	mulsd	%xmm9, %xmm11
	subsd	%xmm12, %xmm11
	subsd	%xmm13, %xmm11
	movapd	%xmm4, %xmm12
	mulsd	%xmm6, %xmm12
	subsd	%xmm12, %xmm10
	mulsd	%xmm4, %xmm8
	subsd	%xmm8, %xmm7
	movapd	%xmm5, %xmm12
	mulsd	%xmm3, %xmm0
	addsd	%xmm7, %xmm0
	mulsd	%xmm3, %xmm9
	movapd	%xmm1, %xmm13
	mulsd	%xmm5, %xmm13
	mulsd	%xmm3, %xmm5
	movapd	%xmm1, %xmm7
	mulsd	%xmm4, %xmm7
	movapd	%xmm4, %xmm8
	mulsd	%xmm3, %xmm8
	addsd	%xmm10, %xmm9
	movapd	%xmm7, %xmm10
	addsd	%xmm5, %xmm10
	unpcklpd	%xmm9, %xmm10                   # xmm10 = xmm10[0],xmm9[0]
	movapd	%xmm13, %xmm9
	subsd	%xmm8, %xmm9
	addsd	%xmm13, %xmm8
	movapd	%xmm1, %xmm13
	mulsd	%xmm1, %xmm13
	mulsd	%xmm2, %xmm2
	mulsd	%xmm3, %xmm6
	mulsd	%xmm4, %xmm12
	mulsd	%xmm3, %xmm1
	addsd	%xmm11, %xmm6
	movapd	%xmm12, %xmm11
	subsd	%xmm1, %xmm11
	unpcklpd	%xmm6, %xmm11                   # xmm11 = xmm11[0],xmm6[0]
	movapd	%xmm13, %xmm14
	subsd	%xmm2, %xmm14
	mulsd	%xmm4, %xmm4
	subsd	%xmm5, %xmm7
	movapd	%xmm2, %xmm6
	subsd	%xmm13, %xmm6
	addsd	%xmm12, %xmm1
	movapd	%xmm4, %xmm5
	subsd	%xmm13, %xmm5
	subsd	%xmm2, %xmm5
	addsd	%xmm13, %xmm2
	subsd	%xmm4, %xmm14
	subsd	%xmm4, %xmm6
	addsd	%xmm4, %xmm2
	mulsd	%xmm3, %xmm3
	addsd	%xmm3, %xmm6
	addsd	%xmm3, %xmm5
	addsd	%xmm3, %xmm2
	unpcklpd	%xmm9, %xmm14                   # xmm14 = xmm14[0],xmm9[0]
	unpcklpd	%xmm9, %xmm3                    # xmm3 = xmm3[0],xmm9[0]
	addpd	%xmm14, %xmm3
	movupd	%xmm3, (%rdi)
	addpd	%xmm10, %xmm10
	movupd	%xmm10, 16(%rdi)
	addsd	%xmm8, %xmm8
	movsd	%xmm8, 32(%rdi)
	movsd	%xmm6, 40(%rdi)
	addpd	%xmm11, %xmm11
	movupd	%xmm11, 48(%rdi)
	addsd	%xmm7, %xmm7
	movsd	%xmm7, 64(%rdi)
	addsd	%xmm1, %xmm1
	movsd	%xmm1, 72(%rdi)
	movsd	%xmm5, 80(%rdi)
	addsd	%xmm0, %xmm0
	movsd	%xmm0, 88(%rdi)
	movsd	%xmm2, 96(%rdi)
	retq
.Lfunc_end7:
	.size	_Z14point_bp_setupRKSt5arrayIdLm8EE, .Lfunc_end7-_Z14point_bp_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE # -- Begin function _Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end8, nop
	.type	_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE,@function
_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE: # @_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	16(%rcx), %xmm1                 # xmm1 = mem[0],zero
	movsd	24(%rcx), %xmm0                 # xmm0 = mem[0],zero
	movupd	(%rcx), %xmm2
	movupd	(%rsi), %xmm3
	movupd	16(%rsi), %xmm4
	movupd	32(%rsi), %xmm5
	movupd	48(%rsi), %xmm6
	movapd	%xmm2, %xmm7
	shufpd	$1, %xmm2, %xmm7                # xmm7 = xmm7[1],xmm2[0]
	movapd	%xmm3, %xmm8
	shufpd	$1, %xmm5, %xmm8                # xmm8 = xmm8[1],xmm5[0]
	mulpd	%xmm7, %xmm8
	movsd	%xmm3, %xmm5                    # xmm5 = xmm3[0],xmm5[1]
	mulpd	%xmm2, %xmm5
	addpd	%xmm8, %xmm5
	movapd	%xmm1, %xmm3
	unpcklpd	%xmm1, %xmm3                    # xmm3 = xmm3[0],xmm1[0]
	movapd	%xmm4, %xmm7
	unpcklpd	%xmm6, %xmm7                    # xmm7 = xmm7[0],xmm6[0]
	mulpd	%xmm3, %xmm7
	addpd	%xmm5, %xmm7
	movapd	%xmm0, %xmm3
	unpcklpd	%xmm0, %xmm3                    # xmm3 = xmm3[0],xmm0[0]
	unpckhpd	%xmm6, %xmm4                    # xmm4 = xmm4[1],xmm6[1]
	mulpd	%xmm3, %xmm4
	addpd	%xmm7, %xmm4
	movupd	%xmm4, (%rdi)
	movsd	64(%rsi), %xmm3                 # xmm3 = mem[0],zero
	mulsd	%xmm2, %xmm3
	unpckhpd	%xmm2, %xmm2                    # xmm2 = xmm2[1,1]
	mulsd	72(%rsi), %xmm2
	addsd	%xmm3, %xmm2
	mulsd	80(%rsi), %xmm1
	addsd	%xmm2, %xmm1
	movsd	88(%rsi), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm0, %xmm2
	addsd	%xmm1, %xmm2
	movsd	%xmm2, 16(%rdi)
	mulsd	96(%rsi), %xmm0
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end8:
	.size	_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE, .Lfunc_end8-_Z14point_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13point_c_setupRKSt5arrayIdLm8EE # -- Begin function _Z13point_c_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end9, nop
	.type	_Z13point_c_setupRKSt5arrayIdLm8EE,@function
_Z13point_c_setupRKSt5arrayIdLm8EE:     # @_Z13point_c_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movupd	(%rsi), %xmm0
	addpd	%xmm0, %xmm0
	movupd	%xmm0, (%rdi)
	movupd	16(%rsi), %xmm0
	addpd	%xmm0, %xmm0
	movupd	%xmm0, 16(%rdi)
	retq
.Lfunc_end9:
	.size	_Z13point_c_setupRKSt5arrayIdLm8EE, .Lfunc_end9-_Z13point_c_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_ # -- Begin function _Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_
	.prefalign	4, .Lfunc_end10, nop
	.type	_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_,@function
_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_: # @_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	24(%rcx), %xmm0                 # xmm0 = mem[0],zero
	movsd	56(%rdx), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm0, %xmm2
	movsd	24(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movupd	(%rdx), %xmm4
	movupd	8(%rdx), %xmm5
	movupd	(%rcx), %xmm3
	movupd	8(%rcx), %xmm6
	movapd	%xmm4, %xmm7
	mulpd	%xmm6, %xmm7
	movapd	%xmm5, %xmm8
	shufpd	$1, %xmm4, %xmm8                # xmm8 = xmm8[1],xmm4[0]
	mulpd	%xmm3, %xmm8
	mulpd	%xmm3, %xmm5
	subpd	%xmm5, %xmm7
	shufpd	$1, %xmm3, %xmm6                # xmm6 = xmm6[1],xmm3[0]
	mulpd	%xmm4, %xmm6
	subpd	%xmm6, %xmm8
	movupd	40(%rdx), %xmm6
	movsd	32(%rdx), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm6, %xmm4
	shufpd	$1, %xmm5, %xmm4                # xmm4 = xmm4[1],xmm5[0]
	movapd	%xmm0, %xmm5
	unpcklpd	%xmm0, %xmm5                    # xmm5 = xmm5[0],xmm0[0]
	mulpd	%xmm5, %xmm4
	addpd	%xmm7, %xmm4
	mulpd	%xmm6, %xmm5
	addpd	%xmm8, %xmm5
	movupd	(%rsi), %xmm7
	movupd	8(%rsi), %xmm6
	movapd	%xmm4, %xmm8
	mulpd	%xmm6, %xmm8
	movapd	%xmm6, %xmm9
	shufpd	$1, %xmm7, %xmm9                # xmm9 = xmm9[1],xmm7[0]
	mulpd	%xmm5, %xmm9
	subpd	%xmm9, %xmm8
	movapd	%xmm4, %xmm9
	shufpd	$1, %xmm5, %xmm9                # xmm9 = xmm9[1],xmm5[0]
	mulsd	%xmm7, %xmm5
	movapd	%xmm4, %xmm10
	unpckhpd	%xmm4, %xmm10                   # xmm10 = xmm10[1],xmm4[1]
	mulsd	%xmm6, %xmm10
	subsd	%xmm10, %xmm5
	mulsd	%xmm1, %xmm4
	unpcklpd	%xmm1, %xmm1                    # xmm1 = xmm1[0,0]
	mulpd	%xmm9, %xmm1
	addpd	%xmm8, %xmm1
	unpckhpd	%xmm6, %xmm6                    # xmm6 = xmm6[1,1]
	mulsd	%xmm2, %xmm6
	unpcklpd	%xmm2, %xmm2                    # xmm2 = xmm2[0,0]
	mulpd	%xmm7, %xmm2
	subpd	%xmm2, %xmm1
	addpd	%xmm3, %xmm1
	addsd	%xmm5, %xmm4
	subsd	%xmm6, %xmm4
	addsd	16(%rcx), %xmm4
	movupd	%xmm1, (%rdi)
	movsd	%xmm4, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end10:
	.size	_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_, .Lfunc_end10-_Z13point_c_applyRKSt5arrayIdLm4EERKS_IdLm8EES2_
	.cfi_endproc
                                        # -- End function
	.globl	_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE # -- Begin function _Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end11, nop
	.type	_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE,@function
_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE: # @_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$128, %rsp
	.cfi_def_cfa_offset 144
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%fs:40, %rax
	movq	%rax, 120(%rsp)
	movups	(%rdx), %xmm0
	movups	16(%rdx), %xmm1
	movups	32(%rdx), %xmm2
	movaps	%xmm2, 96(%rsp)
	movaps	%xmm1, 80(%rsp)
	movaps	%xmm0, 64(%rsp)
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	32(%rsi), %xmm2
	movups	48(%rsi), %xmm3
	movaps	%xmm3, 48(%rsp)
	movaps	%xmm2, 32(%rsp)
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, (%rsp)
	leaq	64(%rsp), %rsi
	movq	%rsp, %rdx
	callq	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	movq	%fs:40, %rax
	cmpq	120(%rsp), %rax
	jne	.LBB11_2
# %bb.1:
	movq	%rbx, %rax
	addq	$128, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB11_2:
	.cfi_def_cfa_offset 144
	callq	__stack_chk_fail@PLT
.Lfunc_end11:
	.size	_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE, .Lfunc_end11-_Z11line_galootRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE # -- Begin function _Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end12, nop
	.type	_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE,@function
_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE:  # @_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rdx), %xmm14                 # xmm14 = mem[0],zero
	movsd	(%rsi), %xmm0                   # xmm0 = mem[0],zero
	movsd	8(%rsi), %xmm2                  # xmm2 = mem[0],zero
	movapd	%xmm0, %xmm3
	mulsd	%xmm1, %xmm3
	movapd	%xmm0, %xmm4
	mulsd	%xmm14, %xmm4
	movsd	16(%rdx), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm0, %xmm6
	mulsd	%xmm5, %xmm6
	movapd	%xmm6, %xmm8
	movsd	24(%rdx), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm0, %xmm11
	mulsd	%xmm9, %xmm11
	movsd	32(%rdx), %xmm13                # xmm13 = mem[0],zero
	movapd	%xmm0, %xmm7
	movapd	%xmm0, %xmm15
	movsd	%xmm0, -72(%rsp)                # 8-byte Spill
	mulsd	%xmm13, %xmm7
	movsd	40(%rdx), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm1, %xmm6
	mulsd	%xmm2, %xmm6
	subsd	%xmm4, %xmm6
	movapd	%xmm6, -56(%rsp)                # 16-byte Spill
	movapd	%xmm14, %xmm4
	mulsd	%xmm2, %xmm4
	addsd	%xmm3, %xmm4
	movsd	%xmm4, -40(%rsp)                # 8-byte Spill
	movapd	%xmm5, %xmm3
	mulsd	%xmm2, %xmm3
	movapd	%xmm9, %xmm4
	mulsd	%xmm2, %xmm4
	subsd	%xmm4, %xmm7
	movapd	%xmm13, %xmm10
	mulsd	%xmm2, %xmm10
	movsd	%xmm2, -64(%rsp)                # 8-byte Spill
	addsd	%xmm11, %xmm10
	movupd	16(%rsi), %xmm4
	movapd	%xmm1, %xmm11
	mulsd	%xmm4, %xmm11
	subsd	%xmm11, %xmm8
	movapd	%xmm14, %xmm12
	mulsd	%xmm4, %xmm12
	subsd	%xmm3, %xmm12
	movapd	%xmm15, %xmm3
	mulsd	%xmm0, %xmm3
	movapd	%xmm9, %xmm11
	mulsd	%xmm4, %xmm11
	subsd	%xmm11, %xmm3
	movapd	%xmm0, %xmm15
	mulsd	%xmm2, %xmm15
	movapd	%xmm13, %xmm11
	mulsd	%xmm4, %xmm11
	subsd	%xmm11, %xmm15
	movapd	%xmm0, %xmm6
	mulsd	%xmm4, %xmm6
	addsd	%xmm10, %xmm6
	movsd	24(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm1, %xmm10
	mulsd	%xmm11, %xmm10
	subsd	%xmm10, %xmm12
	movapd	%xmm14, %xmm10
	mulsd	%xmm11, %xmm10
	subsd	%xmm10, %xmm8
	movsd	%xmm8, -32(%rsp)                # 8-byte Spill
	mulsd	%xmm11, %xmm9
	addsd	%xmm15, %xmm9
	mulsd	%xmm11, %xmm13
	subsd	%xmm13, %xmm3
	mulsd	%xmm11, %xmm0
	addsd	%xmm7, %xmm0
	movupd	32(%rsi), %xmm2
	movapd	%xmm2, -24(%rsp)                # 16-byte Spill
	movapd	%xmm1, %xmm13
	mulsd	%xmm2, %xmm13
	addsd	%xmm6, %xmm13
	movapd	%xmm14, %xmm10
	mulsd	%xmm2, %xmm10
	addsd	%xmm0, %xmm10
	movapd	%xmm5, %xmm15
	mulsd	%xmm2, %xmm15
	addsd	%xmm3, %xmm15
	movsd	40(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm1, %xmm0
	mulsd	%xmm6, %xmm0
	subsd	%xmm0, %xmm10
	movapd	%xmm14, %xmm0
	mulsd	%xmm6, %xmm0
	addsd	%xmm13, %xmm0
	movapd	%xmm5, %xmm8
	mulsd	%xmm6, %xmm8
	addsd	%xmm9, %xmm8
	movupd	48(%rsi), %xmm9
	movapd	%xmm1, %xmm3
	mulsd	%xmm9, %xmm3
	subsd	%xmm3, %xmm15
	movapd	%xmm14, %xmm3
	mulsd	%xmm9, %xmm3
	subsd	%xmm3, %xmm8
	movapd	%xmm5, %xmm3
	mulsd	%xmm9, %xmm3
	addsd	%xmm0, %xmm3
	movsd	56(%rsi), %xmm13                # xmm13 = mem[0],zero
	mulsd	%xmm13, %xmm1
	addsd	%xmm8, %xmm1
	movsd	%xmm1, -80(%rsp)                # 8-byte Spill
	mulsd	%xmm13, %xmm14
	subsd	%xmm14, %xmm15
	movapd	%xmm5, %xmm2
	movapd	%xmm5, %xmm8
	mulsd	%xmm13, %xmm5
	addsd	%xmm10, %xmm5
	mulsd	%xmm4, %xmm2
	addsd	-40(%rsp), %xmm2                # 8-byte Folded Reload
	mulsd	%xmm11, %xmm8
	movapd	-56(%rsp), %xmm0                # 16-byte Reload
	subsd	%xmm8, %xmm0
	movapd	%xmm0, -56(%rsp)                # 16-byte Spill
	movsd	-64(%rsp), %xmm7                # 8-byte Reload
                                        # xmm7 = mem[0],zero
	movapd	%xmm7, %xmm14
	mulsd	%xmm3, %xmm14
	movsd	-72(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm8
	mulsd	%xmm5, %xmm8
	subsd	%xmm8, %xmm14
	movapd	%xmm0, %xmm8
	mulsd	%xmm3, %xmm8
	movapd	%xmm7, %xmm10
	mulsd	%xmm5, %xmm10
	addsd	%xmm8, %xmm10
	mulsd	%xmm4, %xmm3
	mulsd	%xmm11, %xmm5
	addsd	%xmm3, %xmm5
	mulsd	%xmm15, %xmm0
	subsd	%xmm0, %xmm5
	movapd	%xmm4, %xmm3
	mulsd	%xmm15, %xmm3
	addsd	%xmm10, %xmm3
	mulsd	%xmm11, %xmm15
	subsd	%xmm15, %xmm14
	mulsd	%xmm12, %xmm6
	addsd	%xmm5, %xmm6
	movapd	%xmm12, %xmm1
	mulsd	%xmm9, %xmm1
	subsd	%xmm1, %xmm14
	movapd	%xmm7, %xmm8
	mulsd	%xmm12, %xmm8
	movapd	%xmm4, %xmm1
	mulsd	%xmm12, %xmm1
	movapd	%xmm11, %xmm10
	mulsd	%xmm12, %xmm10
	mulsd	%xmm13, %xmm12
	subsd	%xmm12, %xmm3
	movapd	%xmm7, %xmm12
	movsd	-80(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm12
	subsd	%xmm12, %xmm6
	movapd	%xmm4, %xmm12
	mulsd	%xmm0, %xmm12
	addsd	%xmm14, %xmm12
	mulsd	%xmm11, %xmm0
	addsd	%xmm3, %xmm0
	movsd	%xmm0, -80(%rsp)                # 8-byte Spill
	movapd	%xmm4, %xmm15
	movsd	-32(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm15
	subsd	%xmm10, %xmm15
	mulsd	%xmm0, %xmm11
	addsd	%xmm1, %xmm11
	movsd	-72(%rsp), %xmm3                # 8-byte Reload
                                        # xmm3 = mem[0],zero
	movapd	-56(%rsp), %xmm5                # 16-byte Reload
	mulsd	%xmm5, %xmm3
	subsd	%xmm11, %xmm3
	mulsd	%xmm5, %xmm7
	subsd	%xmm7, %xmm15
	movapd	%xmm0, %xmm11
	mulsd	%xmm9, %xmm11
	unpcklpd	%xmm5, %xmm5                    # xmm5 = xmm5[0,0]
	movapd	-24(%rsp), %xmm1                # 16-byte Reload
	movapd	%xmm1, %xmm10
	shufpd	$1, %xmm9, %xmm10               # xmm10 = xmm10[1],xmm9[0]
	movsd	%xmm1, %xmm9                    # xmm9 = xmm1[0],xmm9[1]
	movapd	%xmm4, %xmm14
	unpckhpd	%xmm1, %xmm14                   # xmm14 = xmm14[1],xmm1[1]
	movapd	%xmm1, %xmm7
	mulpd	%xmm5, %xmm14
	mulpd	%xmm5, %xmm9
	unpcklpd	%xmm1, %xmm4                    # xmm4 = xmm4[0],xmm1[0]
	mulsd	%xmm0, %xmm7
	mulsd	%xmm0, %xmm13
	movsd	-72(%rsp), %xmm5                # 8-byte Reload
                                        # xmm5 = mem[0],zero
	mulsd	%xmm5, %xmm0
	mulsd	%xmm2, %xmm5
	addsd	%xmm15, %xmm5
	movsd	-64(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm2, %xmm1
	addsd	%xmm3, %xmm1
	unpcklpd	%xmm2, %xmm2                    # xmm2 = xmm2[0,0]
	mulpd	%xmm2, %xmm4
	mulpd	%xmm10, %xmm2
	subsd	%xmm7, %xmm6
	subsd	%xmm13, %xmm12
	addpd	%xmm9, %xmm12
	unpckhpd	%xmm9, %xmm9                    # xmm9 = xmm9[1,1]
	subpd	%xmm9, %xmm6
	addsd	-80(%rsp), %xmm11               # 8-byte Folded Reload
	subsd	%xmm0, %xmm8
	unpcklpd	%xmm11, %xmm8                   # xmm8 = xmm8[0],xmm11[0]
	subpd	%xmm14, %xmm8
	addpd	%xmm4, %xmm8
	unpcklpd	%xmm6, %xmm12                   # xmm12 = xmm12[0],xmm6[0]
	addpd	%xmm2, %xmm12
	movsd	%xmm5, (%rdi)
	movsd	%xmm1, 8(%rdi)
	movupd	%xmm8, 16(%rdi)
	movupd	%xmm12, 32(%rdi)
	retq
.Lfunc_end12:
	.size	_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE, .Lfunc_end12-_Z6line_aRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE # -- Begin function _Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end13, nop
	.type	_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE,@function
_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE:  # @_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm9                   # xmm9 = mem[0],zero
	movsd	8(%rsi), %xmm3                  # xmm3 = mem[0],zero
	movapd	%xmm9, %xmm4
	mulsd	%xmm9, %xmm4
	movapd	%xmm3, %xmm1
	mulsd	%xmm3, %xmm1
	movsd	16(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm7
	mulsd	%xmm0, %xmm7
	movsd	24(%rsi), %xmm13                # xmm13 = mem[0],zero
	movapd	%xmm13, %xmm2
	mulsd	%xmm13, %xmm2
	movapd	%xmm4, %xmm5
	subsd	%xmm1, %xmm5
	subsd	%xmm7, %xmm5
	addsd	%xmm2, %xmm5
	movsd	%xmm5, -24(%rsp)                # 8-byte Spill
	movapd	%xmm9, %xmm5
	mulsd	%xmm3, %xmm5
	movapd	%xmm0, %xmm6
	mulsd	%xmm13, %xmm6
	movapd	%xmm5, %xmm8
	subsd	%xmm6, %xmm8
	movsd	%xmm8, -48(%rsp)                # 8-byte Spill
	movapd	%xmm6, %xmm11
	movapd	%xmm9, %xmm8
	mulsd	%xmm0, %xmm8
	movapd	%xmm3, %xmm6
	mulsd	%xmm13, %xmm6
	movapd	%xmm8, %xmm12
	movapd	%xmm8, %xmm10
	addsd	%xmm6, %xmm12
	movsd	%xmm12, -40(%rsp)               # 8-byte Spill
	addsd	%xmm5, %xmm11
	movsd	%xmm11, -80(%rsp)               # 8-byte Spill
	movapd	%xmm9, %xmm5
	mulsd	%xmm13, %xmm5
	movapd	%xmm3, %xmm8
	mulsd	%xmm0, %xmm8
	subsd	%xmm6, %xmm10
	movsd	%xmm10, -56(%rsp)               # 8-byte Spill
	movapd	%xmm8, %xmm6
	subsd	%xmm5, %xmm6
	movsd	%xmm6, -72(%rsp)                # 8-byte Spill
	addsd	%xmm5, %xmm8
	movsd	%xmm8, -64(%rsp)                # 8-byte Spill
	movapd	%xmm1, %xmm5
	subsd	%xmm4, %xmm5
	subsd	%xmm7, %xmm5
	addsd	%xmm2, %xmm5
	movsd	%xmm5, -32(%rsp)                # 8-byte Spill
	addsd	%xmm4, %xmm1
	subsd	%xmm1, %xmm7
	addsd	%xmm2, %xmm7
	movsd	%xmm7, -16(%rsp)                # 8-byte Spill
	movsd	32(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movsd	40(%rsi), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm9, %xmm2
	mulsd	%xmm14, %xmm2
	movapd	%xmm3, %xmm4
	mulsd	%xmm1, %xmm4
	addsd	%xmm2, %xmm4
	movsd	56(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm0, %xmm10
	mulsd	%xmm8, %xmm10
	movapd	%xmm4, %xmm2
	subsd	%xmm10, %xmm2
	addsd	%xmm4, %xmm10
	movsd	48(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm13, %xmm4
	mulsd	%xmm6, %xmm4
	subsd	%xmm4, %xmm2
	addsd	%xmm4, %xmm10
	movapd	%xmm13, %xmm5
	mulsd	%xmm8, %xmm5
	movapd	%xmm3, %xmm4
	mulsd	%xmm8, %xmm4
	movapd	%xmm9, %xmm7
	movapd	%xmm9, %xmm11
	mulsd	%xmm8, %xmm9
	movapd	%xmm0, %xmm15
	mulsd	%xmm6, %xmm15
	mulsd	%xmm6, %xmm11
	movapd	%xmm3, %xmm8
	mulsd	%xmm6, %xmm3
	mulsd	%xmm14, %xmm8
	movapd	%xmm13, %xmm6
	mulsd	%xmm14, %xmm6
	movapd	%xmm0, %xmm12
	mulsd	%xmm14, %xmm0
	mulsd	%xmm1, %xmm7
	mulsd	%xmm1, %xmm12
	mulsd	%xmm1, %xmm13
	movapd	%xmm11, %xmm14
	addsd	%xmm4, %xmm14
	subsd	%xmm4, %xmm11
	addsd	%xmm12, %xmm14
	addsd	%xmm12, %xmm11
	addsd	%xmm6, %xmm14
	subsd	%xmm6, %xmm11
	movapd	%xmm3, %xmm1
	subsd	%xmm9, %xmm1
	addsd	%xmm9, %xmm3
	addsd	%xmm0, %xmm1
	addsd	%xmm0, %xmm3
	subsd	%xmm13, %xmm1
	addsd	%xmm13, %xmm3
	movapd	%xmm7, %xmm6
	subsd	%xmm8, %xmm6
	movapd	%xmm8, %xmm12
	subsd	%xmm7, %xmm12
	addsd	%xmm7, %xmm8
	subsd	%xmm15, %xmm6
	subsd	%xmm15, %xmm12
	subsd	%xmm8, %xmm15
	addsd	%xmm5, %xmm6
	addsd	%xmm5, %xmm12
	addsd	%xmm5, %xmm15
	movsd	-48(%rsp), %xmm7                # 8-byte Reload
                                        # xmm7 = mem[0],zero
	addsd	%xmm7, %xmm7
	movsd	%xmm7, -48(%rsp)                # 8-byte Spill
	movsd	(%rdx), %xmm13                  # xmm13 = mem[0],zero
	movsd	-24(%rsp), %xmm4                # 8-byte Reload
                                        # xmm4 = mem[0],zero
	mulsd	%xmm13, %xmm4
	movsd	8(%rdx), %xmm8                  # xmm8 = mem[0],zero
	mulsd	%xmm8, %xmm7
	addsd	%xmm4, %xmm7
	movsd	-40(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm0
	movsd	%xmm0, -40(%rsp)                # 8-byte Spill
	movsd	16(%rdx), %xmm5                 # xmm5 = mem[0],zero
	mulsd	%xmm5, %xmm0
	addsd	%xmm7, %xmm0
	movsd	%xmm0, -8(%rsp)                 # 8-byte Spill
	movsd	-80(%rsp), %xmm4                # 8-byte Reload
                                        # xmm4 = mem[0],zero
	addsd	%xmm4, %xmm4
	movapd	%xmm4, %xmm7
	mulsd	%xmm13, %xmm7
	movsd	-32(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	mulsd	%xmm8, %xmm9
	addsd	%xmm7, %xmm9
	movsd	-72(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm0
	movsd	%xmm0, -72(%rsp)                # 8-byte Spill
	mulsd	%xmm5, %xmm0
	addsd	%xmm9, %xmm0
	movsd	%xmm0, -80(%rsp)                # 8-byte Spill
	movsd	-56(%rsp), %xmm7                # 8-byte Reload
                                        # xmm7 = mem[0],zero
	addsd	%xmm7, %xmm7
	movsd	%xmm7, -56(%rsp)                # 8-byte Spill
	movsd	-64(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm0
	movsd	%xmm0, -64(%rsp)                # 8-byte Spill
	mulsd	%xmm13, %xmm7
	mulsd	%xmm8, %xmm0
	addsd	%xmm7, %xmm0
	movsd	-16(%rsp), %xmm7                # 8-byte Reload
                                        # xmm7 = mem[0],zero
	mulsd	%xmm5, %xmm7
	addsd	%xmm0, %xmm7
	addsd	%xmm6, %xmm6
	addsd	%xmm2, %xmm2
	mulsd	%xmm13, %xmm6
	mulsd	%xmm8, %xmm2
	addsd	%xmm6, %xmm2
	addsd	%xmm14, %xmm14
	mulsd	%xmm5, %xmm14
	addsd	%xmm2, %xmm14
	movsd	24(%rdx), %xmm2                 # xmm2 = mem[0],zero
	movsd	-24(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm2, %xmm0
	addsd	%xmm14, %xmm0
	movapd	%xmm0, %xmm9
	movsd	32(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movsd	-48(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm6, %xmm0
	addsd	%xmm9, %xmm0
	movsd	40(%rdx), %xmm14                # xmm14 = mem[0],zero
	movsd	-40(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	mulsd	%xmm14, %xmm9
	addsd	%xmm0, %xmm9
	addsd	%xmm10, %xmm10
	addsd	%xmm12, %xmm12
	mulsd	%xmm13, %xmm10
	mulsd	%xmm8, %xmm12
	addsd	%xmm10, %xmm12
	addsd	%xmm1, %xmm1
	mulsd	%xmm5, %xmm1
	addsd	%xmm12, %xmm1
	mulsd	%xmm2, %xmm4
	addsd	%xmm1, %xmm4
	movsd	-32(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm6, %xmm0
	addsd	%xmm4, %xmm0
	movsd	-72(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm14, %xmm1
	addsd	%xmm0, %xmm1
	movapd	%xmm1, %xmm4
	addsd	%xmm11, %xmm11
	mulsd	%xmm13, %xmm11
	addsd	%xmm3, %xmm3
	mulsd	%xmm8, %xmm3
	addsd	%xmm11, %xmm3
	addsd	%xmm15, %xmm15
	mulsd	%xmm5, %xmm15
	addsd	%xmm3, %xmm15
	movsd	-56(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm2, %xmm0
	addsd	%xmm15, %xmm0
	movsd	-64(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm6, %xmm1
	addsd	%xmm0, %xmm1
	movsd	-16(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm14, %xmm0
	addsd	%xmm1, %xmm0
	movsd	-8(%rsp), %xmm1                 # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movsd	%xmm1, (%rdi)
	movsd	-80(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movsd	%xmm1, 8(%rdi)
	movsd	%xmm7, 16(%rdi)
	movsd	%xmm9, 24(%rdi)
	movsd	%xmm4, 32(%rdi)
	movsd	%xmm0, 40(%rdi)
	retq
.Lfunc_end13:
	.size	_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE, .Lfunc_end13-_Z6line_bRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE # -- Begin function _Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end14, nop
	.type	_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE,@function
_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE: # @_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm15                  # xmm15 = mem[0],zero
	movsd	8(%rsi), %xmm1                  # xmm1 = mem[0],zero
	movapd	%xmm15, %xmm3
	movapd	%xmm15, %xmm4
	mulsd	%xmm1, %xmm4
	movsd	16(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm15, %xmm0
	mulsd	%xmm7, %xmm0
	movsd	%xmm0, -104(%rsp)               # 8-byte Spill
	movsd	24(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm15, %xmm0
	mulsd	%xmm5, %xmm0
	movsd	%xmm0, -112(%rsp)               # 8-byte Spill
	movsd	32(%rsi), %xmm13                # xmm13 = mem[0],zero
	movapd	%xmm15, %xmm0
	mulsd	%xmm13, %xmm0
	movsd	%xmm0, -88(%rsp)                # 8-byte Spill
	movsd	40(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm15, %xmm6
	mulsd	%xmm11, %xmm6
	movsd	48(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm15, %xmm0
	mulsd	%xmm8, %xmm0
	movsd	%xmm0, -64(%rsp)                # 8-byte Spill
	movsd	56(%rsi), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm15
	movapd	%xmm1, %xmm2
	mulsd	%xmm13, %xmm2
	addsd	%xmm6, %xmm2
	movapd	%xmm1, %xmm14
	mulsd	%xmm7, %xmm14
	movapd	%xmm1, %xmm12
	mulsd	%xmm5, %xmm12
	movapd	%xmm1, %xmm6
	mulsd	%xmm11, %xmm6
	movsd	%xmm6, -24(%rsp)                # 8-byte Spill
	movapd	%xmm1, %xmm6
	mulsd	%xmm8, %xmm6
	movsd	%xmm6, -72(%rsp)                # 8-byte Spill
	movapd	%xmm1, %xmm10
	mulsd	%xmm0, %xmm1
	movapd	%xmm7, %xmm9
	mulsd	%xmm5, %xmm9
	movapd	%xmm7, %xmm6
	mulsd	%xmm13, %xmm6
	movsd	%xmm6, -48(%rsp)                # 8-byte Spill
	movapd	%xmm7, %xmm6
	mulsd	%xmm11, %xmm6
	movsd	%xmm6, -32(%rsp)                # 8-byte Spill
	movapd	%xmm7, %xmm6
	mulsd	%xmm8, %xmm6
	movsd	%xmm7, -96(%rsp)                # 8-byte Spill
	mulsd	%xmm0, %xmm7
	mulsd	%xmm5, %xmm13
	movsd	%xmm13, -40(%rsp)               # 8-byte Spill
	mulsd	%xmm5, %xmm11
	mulsd	%xmm5, %xmm8
	movapd	%xmm5, %xmm13
	mulsd	%xmm0, %xmm5
	movapd	%xmm4, %xmm0
	subsd	%xmm9, %xmm0
	movsd	%xmm0, -80(%rsp)                # 8-byte Spill
	addsd	%xmm4, %xmm9
	movsd	%xmm9, -16(%rsp)                # 8-byte Spill
	movsd	-104(%rsp), %xmm0               # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm9
	addsd	%xmm12, %xmm9
	subsd	%xmm12, %xmm0
	movsd	%xmm0, -104(%rsp)               # 8-byte Spill
	movapd	%xmm14, %xmm0
	movsd	-112(%rsp), %xmm4               # 8-byte Reload
                                        # xmm4 = mem[0],zero
	subsd	%xmm4, %xmm0
	movsd	%xmm0, -8(%rsp)                 # 8-byte Spill
	addsd	%xmm4, %xmm14
	movsd	%xmm14, -112(%rsp)              # 8-byte Spill
	mulsd	%xmm3, %xmm3
	mulsd	%xmm10, %xmm10
	movsd	-96(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm0
	movapd	%xmm3, %xmm12
	subsd	%xmm10, %xmm12
	subsd	%xmm0, %xmm12
	movapd	%xmm10, %xmm4
	subsd	%xmm3, %xmm4
	subsd	%xmm0, %xmm4
	subsd	%xmm3, %xmm0
	subsd	%xmm10, %xmm0
	mulsd	%xmm13, %xmm13
	addsd	%xmm13, %xmm12
	addsd	%xmm13, %xmm4
	movsd	%xmm4, -56(%rsp)                # 8-byte Spill
	addsd	%xmm13, %xmm0
	movsd	%xmm0, -96(%rsp)                # 8-byte Spill
	movapd	%xmm2, %xmm4
	subsd	%xmm7, %xmm4
	addsd	%xmm2, %xmm7
	subsd	%xmm8, %xmm4
	addsd	%xmm8, %xmm7
	movsd	-64(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm8
	addsd	%xmm1, %xmm8
	subsd	%xmm1, %xmm0
	movsd	-48(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	addsd	%xmm1, %xmm8
	addsd	%xmm1, %xmm0
	addsd	%xmm11, %xmm8
	subsd	%xmm11, %xmm0
	movsd	%xmm0, -64(%rsp)                # 8-byte Spill
	movsd	-72(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm3
	subsd	%xmm15, %xmm3
	addsd	%xmm15, %xmm1
	movsd	-32(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm3
	addsd	%xmm0, %xmm1
	movsd	-40(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	subsd	%xmm0, %xmm3
	addsd	%xmm0, %xmm1
	movsd	%xmm1, -72(%rsp)                # 8-byte Spill
	movsd	-88(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm1
	movsd	-24(%rsp), %xmm10               # 8-byte Reload
                                        # xmm10 = mem[0],zero
	subsd	%xmm10, %xmm1
	subsd	%xmm6, %xmm1
	movapd	%xmm10, %xmm2
	subsd	%xmm0, %xmm2
	subsd	%xmm6, %xmm2
	subsd	%xmm0, %xmm6
	subsd	%xmm10, %xmm6
	addsd	%xmm5, %xmm1
	addsd	%xmm5, %xmm2
	addsd	%xmm5, %xmm6
	movsd	-80(%rsp), %xmm15               # 8-byte Reload
                                        # xmm15 = mem[0],zero
	addsd	%xmm15, %xmm15
	movsd	%xmm15, -80(%rsp)               # 8-byte Spill
	movsd	(%rdx), %xmm11                  # xmm11 = mem[0],zero
	movapd	%xmm12, %xmm10
	mulsd	%xmm11, %xmm10
	movsd	8(%rdx), %xmm13                 # xmm13 = mem[0],zero
	mulsd	%xmm13, %xmm15
	addsd	%xmm10, %xmm15
	addsd	%xmm9, %xmm9
	movsd	%xmm9, -88(%rsp)                # 8-byte Spill
	movsd	16(%rdx), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm10, %xmm9
	addsd	%xmm15, %xmm9
	movsd	%xmm9, (%rdi)
	movsd	-16(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	addsd	%xmm9, %xmm9
	movapd	%xmm9, %xmm14
	mulsd	%xmm11, %xmm14
	movsd	-56(%rsp), %xmm15               # 8-byte Reload
                                        # xmm15 = mem[0],zero
	mulsd	%xmm13, %xmm15
	addsd	%xmm14, %xmm15
	movsd	-8(%rsp), %xmm5                 # 8-byte Reload
                                        # xmm5 = mem[0],zero
	addsd	%xmm5, %xmm5
	movapd	%xmm5, %xmm14
	mulsd	%xmm10, %xmm14
	addsd	%xmm15, %xmm14
	movsd	%xmm14, 8(%rdi)
	movsd	-104(%rsp), %xmm14              # 8-byte Reload
                                        # xmm14 = mem[0],zero
	addsd	%xmm14, %xmm14
	movsd	%xmm14, -104(%rsp)              # 8-byte Spill
	movsd	-112(%rsp), %xmm0               # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm0
	movsd	%xmm0, -112(%rsp)               # 8-byte Spill
	mulsd	%xmm11, %xmm14
	mulsd	%xmm13, %xmm0
	addsd	%xmm14, %xmm0
	movsd	-96(%rsp), %xmm14               # 8-byte Reload
                                        # xmm14 = mem[0],zero
	mulsd	%xmm10, %xmm14
	addsd	%xmm0, %xmm14
	movsd	%xmm14, 16(%rdi)
	addsd	%xmm1, %xmm1
	addsd	%xmm4, %xmm4
	mulsd	%xmm11, %xmm1
	mulsd	%xmm13, %xmm4
	addsd	%xmm1, %xmm4
	addsd	%xmm8, %xmm8
	mulsd	%xmm10, %xmm8
	addsd	%xmm4, %xmm8
	movsd	24(%rdx), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm0, %xmm12
	addsd	%xmm8, %xmm12
	movsd	32(%rdx), %xmm4                 # xmm4 = mem[0],zero
	movsd	-80(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm4, %xmm1
	addsd	%xmm12, %xmm1
	movsd	40(%rdx), %xmm8                 # xmm8 = mem[0],zero
	movsd	-88(%rsp), %xmm12               # 8-byte Reload
                                        # xmm12 = mem[0],zero
	mulsd	%xmm8, %xmm12
	addsd	%xmm1, %xmm12
	movsd	%xmm12, 24(%rdi)
	addsd	%xmm7, %xmm7
	addsd	%xmm2, %xmm2
	mulsd	%xmm11, %xmm7
	mulsd	%xmm13, %xmm2
	addsd	%xmm7, %xmm2
	addsd	%xmm3, %xmm3
	mulsd	%xmm10, %xmm3
	addsd	%xmm2, %xmm3
	mulsd	%xmm0, %xmm9
	addsd	%xmm3, %xmm9
	movsd	-56(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm4, %xmm1
	addsd	%xmm9, %xmm1
	mulsd	%xmm8, %xmm5
	addsd	%xmm1, %xmm5
	movsd	%xmm5, 32(%rdi)
	movsd	-64(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	addsd	%xmm1, %xmm1
	mulsd	%xmm11, %xmm1
	movsd	-72(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	addsd	%xmm2, %xmm2
	mulsd	%xmm13, %xmm2
	addsd	%xmm1, %xmm2
	addsd	%xmm6, %xmm6
	mulsd	%xmm10, %xmm6
	addsd	%xmm2, %xmm6
	movsd	-104(%rsp), %xmm1               # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm0, %xmm1
	addsd	%xmm6, %xmm1
	movsd	-112(%rsp), %xmm0               # 8-byte Reload
                                        # xmm0 = mem[0],zero
	mulsd	%xmm4, %xmm0
	addsd	%xmm1, %xmm0
	movsd	-96(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm8, %xmm1
	addsd	%xmm0, %xmm1
	movsd	%xmm1, 40(%rdi)
	retq
.Lfunc_end14:
	.size	_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE, .Lfunc_end14-_Z7line_bpRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE # -- Begin function _Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end15, nop
	.type	_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE,@function
_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE:  # @_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm4                   # xmm4 = mem[0],zero
	movsd	8(%rsi), %xmm9                  # xmm9 = mem[0],zero
	movapd	%xmm9, %xmm0
	addsd	%xmm9, %xmm0
	movsd	16(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm3, %xmm15
	addsd	%xmm3, %xmm15
	movsd	24(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movsd	%xmm7, -32(%rsp)                # 8-byte Spill
	movsd	48(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm9, %xmm5
	mulsd	%xmm8, %xmm5
	movsd	40(%rsi), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm3, %xmm2
	mulsd	%xmm14, %xmm2
	subsd	%xmm2, %xmm5
	movsd	32(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movapd	%xmm3, %xmm10
	mulsd	%xmm2, %xmm10
	movapd	%xmm4, %xmm11
	mulsd	%xmm8, %xmm11
	subsd	%xmm11, %xmm10
	movapd	%xmm4, %xmm11
	mulsd	%xmm14, %xmm11
	movapd	%xmm9, %xmm12
	mulsd	%xmm2, %xmm12
	subsd	%xmm12, %xmm11
	mulsd	%xmm7, %xmm2
	movsd	56(%rsi), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm4, %xmm13
	mulsd	%xmm12, %xmm13
	subsd	%xmm13, %xmm2
	addsd	%xmm5, %xmm2
	mulsd	%xmm7, %xmm14
	movapd	%xmm9, %xmm5
	mulsd	%xmm12, %xmm5
	subsd	%xmm5, %xmm14
	addsd	%xmm10, %xmm14
	mulsd	%xmm8, %xmm7
	mulsd	%xmm3, %xmm12
	subsd	%xmm12, %xmm7
	addsd	%xmm11, %xmm7
	movsd	16(%rdx), %xmm1                 # xmm1 = mem[0],zero
	movsd	%xmm1, -40(%rsp)                # 8-byte Spill
	movapd	%xmm9, %xmm8
	mulsd	%xmm1, %xmm8
	movsd	(%rdx), %xmm12                  # xmm12 = mem[0],zero
	movsd	%xmm12, -24(%rsp)               # 8-byte Spill
	movsd	8(%rdx), %xmm6                  # xmm6 = mem[0],zero
	movsd	%xmm6, -16(%rsp)                # 8-byte Spill
	movapd	%xmm3, %xmm5
	mulsd	%xmm6, %xmm5
	subsd	%xmm5, %xmm8
	movapd	%xmm3, %xmm10
	mulsd	%xmm12, %xmm10
	movapd	%xmm4, %xmm5
	mulsd	%xmm1, %xmm5
	subsd	%xmm5, %xmm10
	movapd	%xmm4, %xmm11
	mulsd	%xmm6, %xmm11
	movapd	%xmm9, %xmm5
	mulsd	%xmm12, %xmm5
	subsd	%xmm5, %xmm11
	movapd	%xmm0, %xmm13
	movsd	%xmm0, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm0, %xmm5
	mulsd	%xmm11, %xmm5
	movapd	%xmm15, %xmm12
	mulsd	%xmm10, %xmm12
	subsd	%xmm12, %xmm5
	movapd	%xmm4, %xmm0
	addsd	%xmm4, %xmm0
	movapd	%xmm15, %xmm6
	mulsd	%xmm8, %xmm6
	movapd	%xmm0, %xmm12
	mulsd	%xmm11, %xmm12
	subsd	%xmm12, %xmm6
	movapd	%xmm0, %xmm12
	mulsd	%xmm10, %xmm12
	mulsd	%xmm8, %xmm13
	subsd	%xmm13, %xmm12
	movsd	-32(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	addsd	%xmm1, %xmm1
	movsd	%xmm1, -32(%rsp)                # 8-byte Spill
	mulsd	%xmm1, %xmm8
	addsd	%xmm5, %xmm8
	addsd	-24(%rsp), %xmm8                # 8-byte Folded Reload
	mulsd	%xmm1, %xmm10
	addsd	%xmm6, %xmm10
	addsd	-16(%rsp), %xmm10               # 8-byte Folded Reload
	mulsd	%xmm1, %xmm11
	addsd	%xmm12, %xmm11
	addsd	-40(%rsp), %xmm11               # 8-byte Folded Reload
	movsd	40(%rdx), %xmm1                 # xmm1 = mem[0],zero
	movapd	%xmm9, %xmm13
	mulsd	%xmm1, %xmm13
	movsd	%xmm1, -40(%rsp)                # 8-byte Spill
	movsd	32(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm3, %xmm5
	mulsd	%xmm6, %xmm5
	subsd	%xmm5, %xmm13
	movsd	24(%rdx), %xmm5                 # xmm5 = mem[0],zero
	mulsd	%xmm5, %xmm3
	movapd	%xmm4, %xmm12
	mulsd	%xmm1, %xmm12
	subsd	%xmm12, %xmm3
	mulsd	%xmm6, %xmm4
	mulsd	%xmm5, %xmm9
	subsd	%xmm9, %xmm4
	movsd	-8(%rsp), %xmm1                 # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm9
	mulsd	%xmm4, %xmm9
	movapd	%xmm15, %xmm12
	mulsd	%xmm3, %xmm12
	subsd	%xmm12, %xmm9
	mulsd	%xmm13, %xmm15
	movapd	%xmm0, %xmm12
	mulsd	%xmm4, %xmm12
	subsd	%xmm12, %xmm15
	mulsd	%xmm3, %xmm0
	mulsd	%xmm13, %xmm1
	subsd	%xmm1, %xmm0
	movsd	-32(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm13
	addsd	%xmm9, %xmm13
	addsd	%xmm5, %xmm13
	mulsd	%xmm1, %xmm3
	addsd	%xmm15, %xmm3
	addsd	%xmm6, %xmm3
	mulsd	%xmm1, %xmm4
	addsd	%xmm14, %xmm14
	addsd	%xmm7, %xmm7
	addsd	%xmm0, %xmm4
	addsd	-40(%rsp), %xmm4                # 8-byte Folded Reload
	movapd	%xmm14, %xmm0
	mulsd	%xmm11, %xmm0
	movapd	%xmm7, %xmm5
	mulsd	%xmm10, %xmm5
	subsd	%xmm5, %xmm0
	addsd	%xmm2, %xmm2
	addsd	%xmm13, %xmm0
	mulsd	%xmm8, %xmm7
	movapd	%xmm2, %xmm5
	mulsd	%xmm11, %xmm5
	subsd	%xmm5, %xmm7
	addsd	%xmm3, %xmm7
	mulsd	%xmm10, %xmm2
	mulsd	%xmm8, %xmm14
	subsd	%xmm14, %xmm2
	addsd	%xmm4, %xmm2
	movsd	%xmm8, (%rdi)
	movsd	%xmm10, 8(%rdi)
	movsd	%xmm11, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	movsd	%xmm7, 32(%rdi)
	movsd	%xmm2, 40(%rdi)
	retq
.Lfunc_end15:
	.size	_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE, .Lfunc_end15-_Z6line_cRKSt5arrayIdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z12line_b_setupRKSt5arrayIdLm8EE # -- Begin function _Z12line_b_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end16, nop
	.type	_Z12line_b_setupRKSt5arrayIdLm8EE,@function
_Z12line_b_setupRKSt5arrayIdLm8EE:      # @_Z12line_b_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movupd	(%rsi), %xmm10
	movupd	16(%rsi), %xmm1
	movupd	32(%rsi), %xmm2
	movapd	%xmm1, %xmm0
	unpckhpd	%xmm1, %xmm0                    # xmm0 = xmm0[1],xmm1[1]
	movapd	%xmm0, %xmm9
	mulsd	%xmm0, %xmm9
	movapd	%xmm1, %xmm6
	mulsd	%xmm0, %xmm6
	movapd	%xmm0, -24(%rsp)                # 16-byte Spill
	movapd	%xmm10, %xmm7
	unpckhpd	%xmm10, %xmm7                   # xmm7 = xmm7[1],xmm10[1]
	mulsd	%xmm10, %xmm7
	movapd	%xmm10, %xmm11
	movapd	%xmm10, %xmm8
	movapd	%xmm10, %xmm13
	movapd	%xmm10, %xmm14
	mulpd	%xmm10, %xmm14
	movapd	%xmm14, %xmm15
	unpckhpd	%xmm14, %xmm15                  # xmm15 = xmm15[1],xmm14[1]
	movapd	%xmm14, %xmm3
	subsd	%xmm15, %xmm3
	movapd	%xmm3, -40(%rsp)                # 16-byte Spill
	movapd	%xmm1, %xmm3
	unpcklpd	%xmm1, %xmm3                    # xmm3 = xmm3[0],xmm1[0]
	mulpd	%xmm3, %xmm11
	movapd	%xmm11, %xmm5
	unpcklpd	%xmm7, %xmm11                   # xmm11 = xmm11[0],xmm7[0]
	subsd	%xmm6, %xmm7
	shufpd	$1, %xmm5, %xmm5                # xmm5 = xmm5[1,0]
	mulpd	%xmm0, %xmm8
	movapd	%xmm5, %xmm12
	addpd	%xmm8, %xmm12
	subpd	%xmm8, %xmm5
	shufpd	$1, %xmm6, %xmm8                # xmm8 = xmm8[1],xmm6[0]
	movapd	%xmm15, %xmm6
	subsd	%xmm14, %xmm6
	addsd	%xmm14, %xmm15
	movapd	%xmm1, %xmm4
	mulsd	%xmm1, %xmm4
	movapd	-40(%rsp), %xmm0                # 16-byte Reload
	subsd	%xmm4, %xmm0
	subsd	%xmm4, %xmm6
	subsd	%xmm15, %xmm4
	movapd	%xmm9, %xmm14
	addsd	%xmm9, %xmm4
	movapd	%xmm9, %xmm15
	unpcklpd	%xmm7, %xmm15                   # xmm15 = xmm15[0],xmm7[0]
	addpd	%xmm11, %xmm8
	movapd	%xmm8, -40(%rsp)                # 16-byte Spill
	unpcklpd	%xmm7, %xmm0                    # xmm0 = xmm0[0],xmm7[0]
	movapd	%xmm1, %xmm11
	mulpd	%xmm2, %xmm11
	shufpd	$1, %xmm1, %xmm1                # xmm1 = xmm1[1,0]
	mulpd	%xmm2, %xmm1
	movapd	%xmm1, %xmm8
	movapd	%xmm2, %xmm9
	unpcklpd	%xmm2, %xmm2                    # xmm2 = xmm2[0,0]
	mulpd	%xmm10, %xmm2
	addpd	%xmm15, %xmm0
	movapd	%xmm0, %xmm1
	movupd	48(%rsi), %xmm7
	unpcklpd	%xmm5, %xmm14                   # xmm14 = xmm14[0],xmm5[0]
	unpcklpd	%xmm5, %xmm6                    # xmm6 = xmm6[0],xmm5[0]
	shufpd	$1, %xmm12, %xmm5               # xmm5 = xmm5[1],xmm12[0]
	addpd	%xmm14, %xmm6
	shufpd	$1, %xmm10, %xmm13              # xmm13 = xmm13[1],xmm10[0]
	unpckhpd	%xmm9, %xmm9                    # xmm9 = xmm9[1,1]
	mulpd	%xmm13, %xmm9
	mulpd	%xmm7, %xmm13
	mulpd	%xmm7, %xmm10
	movapd	%xmm10, %xmm14
	unpckhpd	%xmm10, %xmm14                  # xmm14 = xmm14[1],xmm10[1]
	movapd	%xmm2, %xmm12
	subpd	%xmm9, %xmm12
	mulpd	%xmm7, %xmm3
	movapd	-24(%rsp), %xmm0                # 16-byte Reload
	mulpd	%xmm7, %xmm0
	movapd	%xmm2, %xmm15
	addpd	%xmm9, %xmm15
	subsd	%xmm2, %xmm9
	movapd	%xmm10, %xmm7
	addsd	%xmm14, %xmm7
	shufpd	$2, %xmm15, %xmm12              # xmm12 = xmm12[0],xmm15[1]
	subpd	%xmm3, %xmm12
	shufpd	$2, %xmm3, %xmm7                # xmm7 = xmm7[0],xmm3[1]
	subsd	%xmm3, %xmm9
	subsd	%xmm15, %xmm3
	movsd	%xmm11, %xmm15                  # xmm15 = xmm11[0],xmm15[1]
	addpd	%xmm15, %xmm7
	movapd	%xmm11, %xmm2
	shufpd	$1, %xmm0, %xmm2                # xmm2 = xmm2[1],xmm0[0]
	addpd	%xmm7, %xmm2
	movapd	%xmm0, %xmm7
	shufpd	$1, %xmm0, %xmm7                # xmm7 = xmm7[1],xmm0[0]
	movapd	%xmm12, %xmm15
	addpd	%xmm7, %xmm15
	subpd	%xmm7, %xmm12
	movsd	%xmm15, %xmm12                  # xmm12 = xmm15[0],xmm12[1]
	movapd	%xmm13, %xmm7
	unpckhpd	%xmm13, %xmm7                   # xmm7 = xmm7[1],xmm13[1]
	subsd	%xmm14, %xmm10
	movapd	%xmm13, %xmm14
	addsd	%xmm7, %xmm14
	subsd	%xmm7, %xmm13
	movapd	%xmm8, %xmm7
	unpckhpd	%xmm8, %xmm7                    # xmm7 = xmm7[1],xmm8[1]
	addsd	%xmm7, %xmm14
	addsd	%xmm13, %xmm7
	addsd	%xmm11, %xmm10
	unpckhpd	%xmm11, %xmm11                  # xmm11 = xmm11[1,1]
	subsd	%xmm11, %xmm10
	addsd	%xmm8, %xmm14
	unpcklpd	%xmm14, %xmm10                  # xmm10 = xmm10[0],xmm14[0]
	subsd	%xmm8, %xmm7
	unpckhpd	%xmm0, %xmm0                    # xmm0 = xmm0[1,1]
	addsd	%xmm0, %xmm9
	unpcklpd	%xmm7, %xmm9                    # xmm9 = xmm9[0],xmm7[0]
	addsd	%xmm0, %xmm3
	movapd	-40(%rsp), %xmm0                # 16-byte Reload
	addpd	%xmm0, %xmm0
	movupd	%xmm0, 16(%rdi)
	movupd	%xmm1, (%rdi)
	addpd	%xmm5, %xmm5
	movupd	%xmm5, 48(%rdi)
	movupd	%xmm6, 32(%rdi)
	movsd	%xmm4, 64(%rdi)
	addpd	%xmm12, %xmm12
	movupd	%xmm12, 72(%rdi)
	addpd	%xmm2, %xmm2
	movupd	%xmm2, 88(%rdi)
	addpd	%xmm9, %xmm9
	movupd	%xmm9, 104(%rdi)
	addpd	%xmm10, %xmm10
	movupd	%xmm10, 120(%rdi)
	addsd	%xmm3, %xmm3
	movsd	%xmm3, 136(%rdi)
	retq
.Lfunc_end16:
	.size	_Z12line_b_setupRKSt5arrayIdLm8EE, .Lfunc_end16-_Z12line_b_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE # -- Begin function _Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end17, nop
	.type	_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE,@function
_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE: # @_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rcx), %xmm3                   # xmm3 = mem[0],zero
	movsd	8(%rcx), %xmm2                  # xmm2 = mem[0],zero
	movsd	(%rsi), %xmm14                  # xmm14 = mem[0],zero
	movsd	8(%rsi), %xmm13                 # xmm13 = mem[0],zero
	movapd	%xmm3, %xmm0
	mulsd	%xmm14, %xmm0
	movapd	%xmm2, %xmm5
	mulsd	%xmm13, %xmm5
	addsd	%xmm0, %xmm5
	movsd	16(%rcx), %xmm4                 # xmm4 = mem[0],zero
	movsd	16(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm4, %xmm0
	mulsd	%xmm6, %xmm0
	addsd	%xmm5, %xmm0
	movsd	%xmm0, -8(%rsp)                 # 8-byte Spill
	movsd	24(%rsi), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm3, %xmm0
	mulsd	%xmm12, %xmm0
	movsd	32(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm2, %xmm7
	mulsd	%xmm11, %xmm7
	addsd	%xmm0, %xmm7
	movsd	40(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm4, %xmm0
	mulsd	%xmm5, %xmm0
	addsd	%xmm7, %xmm0
	movsd	%xmm0, -16(%rsp)                # 8-byte Spill
	movsd	48(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movapd	%xmm3, %xmm0
	mulsd	%xmm9, %xmm0
	movsd	56(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm2, %xmm15
	mulsd	%xmm8, %xmm15
	addsd	%xmm0, %xmm15
	movsd	64(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movapd	%xmm4, %xmm10
	mulsd	%xmm7, %xmm10
	addsd	%xmm15, %xmm10
	movsd	72(%rsi), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm3, %xmm0
	movsd	80(%rsi), %xmm15                # xmm15 = mem[0],zero
	mulsd	%xmm2, %xmm15
	addsd	%xmm0, %xmm15
	movsd	88(%rsi), %xmm0                 # xmm0 = mem[0],zero
	mulsd	%xmm4, %xmm0
	addsd	%xmm15, %xmm0
	movsd	24(%rcx), %xmm15                # xmm15 = mem[0],zero
	mulsd	%xmm15, %xmm14
	addsd	%xmm0, %xmm14
	movsd	32(%rcx), %xmm1                 # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm13
	addsd	%xmm14, %xmm13
	movsd	40(%rcx), %xmm14                # xmm14 = mem[0],zero
	mulsd	%xmm14, %xmm6
	addsd	%xmm13, %xmm6
	movsd	96(%rsi), %xmm13                # xmm13 = mem[0],zero
	mulsd	%xmm3, %xmm13
	movsd	104(%rsi), %xmm0                # xmm0 = mem[0],zero
	mulsd	%xmm2, %xmm0
	addsd	%xmm13, %xmm0
	movsd	112(%rsi), %xmm13               # xmm13 = mem[0],zero
	mulsd	%xmm4, %xmm13
	addsd	%xmm0, %xmm13
	mulsd	%xmm15, %xmm12
	addsd	%xmm13, %xmm12
	mulsd	%xmm1, %xmm11
	addsd	%xmm12, %xmm11
	mulsd	%xmm14, %xmm5
	mulsd	120(%rsi), %xmm3
	addsd	%xmm11, %xmm5
	mulsd	128(%rsi), %xmm2
	addsd	%xmm3, %xmm2
	mulsd	136(%rsi), %xmm4
	addsd	%xmm2, %xmm4
	mulsd	%xmm15, %xmm9
	addsd	%xmm4, %xmm9
	mulsd	%xmm1, %xmm8
	addsd	%xmm9, %xmm8
	mulsd	%xmm14, %xmm7
	addsd	%xmm8, %xmm7
	movsd	-8(%rsp), %xmm0                 # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movsd	%xmm0, (%rdi)
	movsd	-16(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movsd	%xmm0, 8(%rdi)
	movsd	%xmm10, 16(%rdi)
	movsd	%xmm6, 24(%rdi)
	movsd	%xmm5, 32(%rdi)
	movsd	%xmm7, 40(%rdi)
	retq
.Lfunc_end17:
	.size	_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE, .Lfunc_end17-_Z12line_b_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13line_bp_setupRKSt5arrayIdLm8EE # -- Begin function _Z13line_bp_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end18, nop
	.type	_Z13line_bp_setupRKSt5arrayIdLm8EE,@function
_Z13line_bp_setupRKSt5arrayIdLm8EE:     # @_Z13line_bp_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	24(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movsd	16(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm0, %xmm1
	mulsd	%xmm5, %xmm1
	movapd	%xmm1, %xmm10
	movsd	32(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movsd	56(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movsd	48(%rsi), %xmm13                # xmm13 = mem[0],zero
	movsd	40(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movsd	(%rsi), %xmm15                  # xmm15 = mem[0],zero
	movsd	8(%rsi), %xmm8                  # xmm8 = mem[0],zero
	movapd	%xmm15, %xmm4
	mulsd	%xmm1, %xmm4
	movapd	%xmm8, %xmm11
	mulsd	%xmm7, %xmm11
	addsd	%xmm4, %xmm11
	movapd	%xmm15, %xmm4
	mulsd	%xmm13, %xmm4
	movapd	%xmm8, %xmm2
	mulsd	%xmm1, %xmm2
	movsd	%xmm2, -32(%rsp)                # 8-byte Spill
	movapd	%xmm8, %xmm9
	mulsd	%xmm13, %xmm9
	movapd	%xmm8, %xmm2
	mulsd	%xmm3, %xmm2
	movsd	%xmm2, -96(%rsp)                # 8-byte Spill
	movapd	%xmm15, %xmm2
	mulsd	%xmm3, %xmm2
	movsd	%xmm2, -72(%rsp)                # 8-byte Spill
	movapd	%xmm15, %xmm2
	mulsd	%xmm7, %xmm2
	movsd	%xmm2, -40(%rsp)                # 8-byte Spill
	movapd	%xmm0, %xmm2
	mulsd	%xmm7, %xmm2
	movsd	%xmm2, -80(%rsp)                # 8-byte Spill
	movapd	%xmm0, %xmm2
	mulsd	%xmm1, %xmm2
	movsd	%xmm2, -64(%rsp)                # 8-byte Spill
	movapd	%xmm0, %xmm6
	mulsd	%xmm13, %xmm6
	movapd	%xmm0, %xmm2
	mulsd	%xmm3, %xmm2
	movsd	%xmm2, -104(%rsp)               # 8-byte Spill
	mulsd	%xmm5, %xmm13
	mulsd	%xmm5, %xmm1
	movsd	%xmm1, -88(%rsp)                # 8-byte Spill
	mulsd	%xmm5, %xmm7
	movsd	%xmm7, -56(%rsp)                # 8-byte Spill
	mulsd	%xmm5, %xmm3
	movsd	%xmm3, -48(%rsp)                # 8-byte Spill
	movapd	%xmm8, %xmm3
	mulsd	%xmm5, %xmm3
	movapd	%xmm15, %xmm1
	mulsd	%xmm8, %xmm1
	movapd	%xmm1, %xmm12
	movapd	%xmm10, %xmm2
	addsd	%xmm10, %xmm12
	movapd	%xmm5, %xmm10
	mulsd	%xmm15, %xmm5
	movapd	%xmm15, %xmm7
	mulsd	%xmm0, %xmm15
	movapd	%xmm15, %xmm14
	addsd	%xmm3, %xmm14
	unpcklpd	%xmm12, %xmm14                  # xmm14 = xmm14[0],xmm12[0]
	subsd	%xmm2, %xmm1
	movapd	%xmm1, -24(%rsp)                # 16-byte Spill
	subsd	%xmm3, %xmm15
	movapd	%xmm8, %xmm12
	mulsd	%xmm0, %xmm8
	movapd	%xmm8, %xmm3
	addsd	%xmm5, %xmm3
	unpcklpd	%xmm3, %xmm15                   # xmm15 = xmm15[0],xmm3[0]
	subsd	%xmm5, %xmm8
	movapd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm2
	mulsd	%xmm7, %xmm7
	mulsd	%xmm12, %xmm12
	movapd	%xmm7, %xmm3
	subsd	%xmm12, %xmm3
	movapd	%xmm12, %xmm5
	subsd	%xmm7, %xmm5
	subsd	%xmm2, %xmm3
	subsd	%xmm2, %xmm5
	subsd	%xmm7, %xmm2
	subsd	%xmm12, %xmm2
	movapd	%xmm11, %xmm7
	movsd	-104(%rsp), %xmm0               # 8-byte Reload
                                        # xmm0 = mem[0],zero
	subsd	%xmm0, %xmm7
	addsd	%xmm11, %xmm0
	subsd	%xmm13, %xmm7
	addsd	%xmm13, %xmm0
	movsd	%xmm0, -104(%rsp)               # 8-byte Spill
	movapd	%xmm4, %xmm11
	movsd	-96(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm11
	subsd	%xmm0, %xmm4
	movapd	%xmm9, %xmm12
	movsd	-72(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm12
	subsd	%xmm0, %xmm9
	movsd	-80(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm11
	addsd	%xmm0, %xmm4
	movsd	-64(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm12
	addsd	%xmm0, %xmm9
	movsd	-88(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm11
	subsd	%xmm0, %xmm4
	movsd	-56(%rsp), %xmm13               # 8-byte Reload
                                        # xmm13 = mem[0],zero
	addsd	%xmm13, %xmm12
	unpcklpd	%xmm12, %xmm4                   # xmm4 = xmm4[0],xmm12[0]
	subsd	%xmm13, %xmm9
	movsd	-32(%rsp), %xmm1                # 8-byte Reload
                                        # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm13
	movsd	-40(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	subsd	%xmm0, %xmm13
	subsd	%xmm6, %xmm13
	movsd	-48(%rsp), %xmm12               # 8-byte Reload
                                        # xmm12 = mem[0],zero
	addsd	%xmm12, %xmm13
	unpcklpd	%xmm9, %xmm13                   # xmm13 = xmm13[0],xmm9[0]
	movapd	%xmm0, %xmm9
	subsd	%xmm1, %xmm9
	subsd	%xmm6, %xmm9
	subsd	%xmm0, %xmm6
	subsd	%xmm1, %xmm6
	mulsd	%xmm10, %xmm10
	addsd	%xmm12, %xmm9
	addsd	%xmm12, %xmm6
	movapd	-24(%rsp), %xmm0                # 16-byte Reload
	unpcklpd	%xmm0, %xmm3                    # xmm3 = xmm3[0],xmm0[0]
	movapd	%xmm10, %xmm12
	unpcklpd	%xmm0, %xmm12                   # xmm12 = xmm12[0],xmm0[0]
	addpd	%xmm3, %xmm12
	addpd	%xmm14, %xmm14
	movupd	%xmm14, 16(%rdi)
	movupd	%xmm12, (%rdi)
	addsd	%xmm10, %xmm2
	unpcklpd	%xmm8, %xmm5                    # xmm5 = xmm5[0],xmm8[0]
	unpcklpd	%xmm8, %xmm10                   # xmm10 = xmm10[0],xmm8[0]
	addpd	%xmm5, %xmm10
	addpd	%xmm15, %xmm15
	movupd	%xmm15, 48(%rdi)
	movupd	%xmm10, 32(%rdi)
	movsd	%xmm2, 64(%rdi)
	addsd	%xmm9, %xmm9
	movsd	%xmm9, 72(%rdi)
	addsd	%xmm7, %xmm7
	movsd	%xmm7, 80(%rdi)
	addsd	%xmm11, %xmm11
	movsd	%xmm11, 88(%rdi)
	movsd	-104(%rsp), %xmm0               # 8-byte Reload
                                        # xmm0 = mem[0],zero
	addsd	%xmm0, %xmm0
	movsd	%xmm0, 96(%rdi)
	addpd	%xmm4, %xmm4
	movupd	%xmm4, 120(%rdi)
	addpd	%xmm13, %xmm13
	movupd	%xmm13, 104(%rdi)
	addsd	%xmm6, %xmm6
	movsd	%xmm6, 136(%rdi)
	retq
.Lfunc_end18:
	.size	_Z13line_bp_setupRKSt5arrayIdLm8EE, .Lfunc_end18-_Z13line_bp_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE # -- Begin function _Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end19, nop
	.type	_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE,@function
_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE: # @_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm10                  # xmm10 = mem[0],zero
	movsd	8(%rsi), %xmm9                  # xmm9 = mem[0],zero
	movsd	(%rcx), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rcx), %xmm0                  # xmm0 = mem[0],zero
	movapd	%xmm10, %xmm2
	mulsd	%xmm1, %xmm2
	movapd	%xmm9, %xmm3
	mulsd	%xmm0, %xmm3
	addsd	%xmm2, %xmm3
	movsd	16(%rsi), %xmm11                # xmm11 = mem[0],zero
	movsd	16(%rcx), %xmm2                 # xmm2 = mem[0],zero
	movapd	%xmm11, %xmm4
	mulsd	%xmm2, %xmm4
	addsd	%xmm3, %xmm4
	movsd	%xmm4, (%rdi)
	movsd	24(%rsi), %xmm7                 # xmm7 = mem[0],zero
	movsd	32(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm1, %xmm3
	mulsd	%xmm7, %xmm3
	movapd	%xmm0, %xmm4
	mulsd	%xmm6, %xmm4
	addsd	%xmm3, %xmm4
	movsd	40(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm2, %xmm3
	mulsd	%xmm8, %xmm3
	addsd	%xmm4, %xmm3
	movsd	%xmm3, 8(%rdi)
	movsd	48(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movsd	56(%rsi), %xmm3                 # xmm3 = mem[0],zero
	movapd	%xmm1, %xmm5
	mulsd	%xmm4, %xmm5
	movapd	%xmm0, %xmm12
	mulsd	%xmm3, %xmm12
	addsd	%xmm5, %xmm12
	movsd	64(%rsi), %xmm5                 # xmm5 = mem[0],zero
	movapd	%xmm2, %xmm13
	mulsd	%xmm5, %xmm13
	addsd	%xmm12, %xmm13
	movsd	%xmm13, 16(%rdi)
	movsd	72(%rsi), %xmm12                # xmm12 = mem[0],zero
	mulsd	%xmm1, %xmm12
	movsd	80(%rsi), %xmm13                # xmm13 = mem[0],zero
	mulsd	%xmm0, %xmm13
	addsd	%xmm12, %xmm13
	movsd	88(%rsi), %xmm14                # xmm14 = mem[0],zero
	mulsd	%xmm2, %xmm14
	addsd	%xmm13, %xmm14
	movsd	24(%rcx), %xmm13                # xmm13 = mem[0],zero
	movsd	32(%rcx), %xmm12                # xmm12 = mem[0],zero
	mulsd	%xmm13, %xmm10
	addsd	%xmm14, %xmm10
	mulsd	%xmm12, %xmm9
	addsd	%xmm10, %xmm9
	movsd	40(%rcx), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm10, %xmm11
	addsd	%xmm9, %xmm11
	movsd	%xmm11, 24(%rdi)
	movsd	96(%rsi), %xmm9                 # xmm9 = mem[0],zero
	mulsd	%xmm1, %xmm9
	movsd	104(%rsi), %xmm11               # xmm11 = mem[0],zero
	mulsd	%xmm0, %xmm11
	addsd	%xmm9, %xmm11
	movsd	112(%rsi), %xmm9                # xmm9 = mem[0],zero
	mulsd	%xmm2, %xmm9
	addsd	%xmm11, %xmm9
	mulsd	%xmm13, %xmm7
	addsd	%xmm9, %xmm7
	mulsd	%xmm12, %xmm6
	addsd	%xmm7, %xmm6
	mulsd	%xmm10, %xmm8
	addsd	%xmm6, %xmm8
	movsd	%xmm8, 32(%rdi)
	mulsd	120(%rsi), %xmm1
	mulsd	128(%rsi), %xmm0
	addsd	%xmm1, %xmm0
	mulsd	136(%rsi), %xmm2
	addsd	%xmm0, %xmm2
	mulsd	%xmm13, %xmm4
	addsd	%xmm2, %xmm4
	mulsd	%xmm12, %xmm3
	addsd	%xmm4, %xmm3
	mulsd	%xmm10, %xmm5
	addsd	%xmm3, %xmm5
	movsd	%xmm5, 40(%rdi)
	retq
.Lfunc_end19:
	.size	_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE, .Lfunc_end19-_Z13line_bp_applyRKSt5arrayIdLm18EERKS_IdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z12line_c_setupRKSt5arrayIdLm8EE # -- Begin function _Z12line_c_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end20, nop
	.type	_Z12line_c_setupRKSt5arrayIdLm8EE,@function
_Z12line_c_setupRKSt5arrayIdLm8EE:      # @_Z12line_c_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	56(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movupd	(%rsi), %xmm3
	movapd	%xmm3, %xmm0
	addpd	%xmm3, %xmm0
	movupd	%xmm0, (%rdi)
	movupd	16(%rsi), %xmm0
	movapd	%xmm0, %xmm4
	addpd	%xmm0, %xmm4
	movapd	%xmm0, %xmm2
	mulsd	%xmm1, %xmm2
	movupd	%xmm4, 16(%rdi)
	movupd	32(%rsi), %xmm5
	movupd	40(%rsi), %xmm6
	movapd	%xmm3, %xmm7
	unpckhpd	%xmm3, %xmm7                    # xmm7 = xmm7[1],xmm3[1]
	mulsd	%xmm5, %xmm7
	movapd	%xmm3, %xmm4
	mulsd	%xmm6, %xmm4
	subsd	%xmm7, %xmm4
	unpcklpd	%xmm1, %xmm1                    # xmm1 = xmm1[0,0]
	mulpd	%xmm3, %xmm1
	movapd	%xmm0, %xmm7
	unpcklpd	%xmm3, %xmm7                    # xmm7 = xmm7[0],xmm3[0]
	mulpd	%xmm6, %xmm7
	shufpd	$1, %xmm0, %xmm3                # xmm3 = xmm3[1],xmm0[0]
	movapd	%xmm0, %xmm8
	mulpd	%xmm6, %xmm0
	shufpd	$1, %xmm5, %xmm6                # xmm6 = xmm6[1],xmm5[0]
	mulpd	%xmm3, %xmm6
	subpd	%xmm7, %xmm6
	unpckhpd	%xmm8, %xmm8                    # xmm8 = xmm8[1,1]
	mulpd	%xmm5, %xmm8
	subpd	%xmm1, %xmm8
	addpd	%xmm6, %xmm8
	unpckhpd	%xmm0, %xmm0                    # xmm0 = xmm0[1,1]
	subsd	%xmm2, %xmm0
	addsd	%xmm4, %xmm0
	addpd	%xmm8, %xmm8
	addsd	%xmm0, %xmm0
	movupd	%xmm8, 32(%rdi)
	movsd	%xmm0, 48(%rdi)
	retq
.Lfunc_end20:
	.size	_Z12line_c_setupRKSt5arrayIdLm8EE, .Lfunc_end20-_Z12line_c_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE # -- Begin function _Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE
	.prefalign	4, .Lfunc_end21, nop
	.type	_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE,@function
_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE: # @_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movupd	(%rdx), %xmm0
	movupd	8(%rdx), %xmm11
	movapd	%xmm11, %xmm12
	shufpd	$1, %xmm0, %xmm12               # xmm12 = xmm12[1],xmm0[0]
	movupd	(%rcx), %xmm1
	movupd	8(%rcx), %xmm4
	movupd	24(%rcx), %xmm3
	movapd	%xmm4, %xmm5
	shufpd	$1, %xmm1, %xmm5                # xmm5 = xmm5[1],xmm1[0]
	mulpd	%xmm12, %xmm1
	movapd	%xmm11, %xmm13
	mulpd	%xmm5, %xmm13
	movapd	%xmm4, %xmm2
	mulpd	%xmm12, %xmm2
	subpd	%xmm2, %xmm13
	movapd	%xmm0, %xmm2
	mulpd	%xmm5, %xmm2
	subpd	%xmm2, %xmm1
	movupd	(%rsi), %xmm7
	movupd	8(%rsi), %xmm6
	movapd	%xmm1, %xmm9
	mulpd	%xmm7, %xmm9
	movapd	%xmm6, %xmm8
	shufpd	$1, %xmm7, %xmm8                # xmm8 = xmm8[1],xmm7[0]
	movapd	%xmm13, %xmm10
	mulpd	%xmm8, %xmm10
	movapd	%xmm1, %xmm2
	shufpd	$1, %xmm13, %xmm2               # xmm2 = xmm2[1],xmm13[0]
	mulpd	%xmm6, %xmm13
	subpd	%xmm13, %xmm9
	movapd	%xmm2, %xmm13
	mulpd	%xmm7, %xmm13
	subpd	%xmm13, %xmm10
	movupd	32(%rcx), %xmm13
	mulpd	%xmm13, %xmm12
	mulpd	%xmm13, %xmm0
	shufpd	$1, %xmm3, %xmm13               # xmm13 = xmm13[1],xmm3[0]
	mulpd	%xmm11, %xmm13
	subpd	%xmm12, %xmm13
	movsd	24(%rsi), %xmm14                # xmm14 = mem[0],zero
	mulpd	%xmm3, %xmm11
	subpd	%xmm11, %xmm0
	mulpd	%xmm0, %xmm6
	movapd	%xmm13, %xmm12
	shufpd	$1, %xmm0, %xmm12               # xmm12 = xmm12[1],xmm0[0]
	mulsd	%xmm14, %xmm0
	movapd	%xmm14, %xmm11
	unpcklpd	%xmm14, %xmm11                  # xmm11 = xmm11[0],xmm14[0]
	mulpd	%xmm11, %xmm2
	addpd	%xmm9, %xmm2
	mulpd	%xmm11, %xmm1
	addpd	%xmm10, %xmm1
	addpd	%xmm5, %xmm2
	addpd	%xmm4, %xmm1
	mulpd	%xmm8, %xmm12
	subpd	%xmm12, %xmm6
	movapd	%xmm7, %xmm4
	unpckhpd	%xmm7, %xmm4                    # xmm4 = xmm4[1],xmm7[1]
	mulsd	%xmm13, %xmm4
	mulpd	%xmm13, %xmm11
	unpckhpd	%xmm13, %xmm13                  # xmm13 = xmm13[1,1]
	mulsd	%xmm7, %xmm13
	subsd	%xmm4, %xmm13
	addpd	%xmm6, %xmm11
	movsd	32(%rsi), %xmm4                 # xmm4 = mem[0],zero
	addpd	%xmm3, %xmm11
	addsd	%xmm13, %xmm0
	addsd	40(%rcx), %xmm0
	movupd	40(%rsi), %xmm3
	movapd	%xmm2, %xmm5
	shufpd	$1, %xmm1, %xmm5                # xmm5 = xmm5[1],xmm1[0]
	movupd	%xmm5, (%rdi)
	movapd	%xmm2, %xmm5
	movlpd	%xmm2, 16(%rdi)
	mulpd	%xmm3, %xmm2
	unpckhpd	%xmm5, %xmm5                    # xmm5 = xmm5[1,1]
	mulsd	%xmm3, %xmm5
	shufpd	$1, %xmm4, %xmm3                # xmm3 = xmm3[1],xmm4[0]
	mulpd	%xmm1, %xmm3
	subpd	%xmm3, %xmm2
	addpd	%xmm11, %xmm2
	mulsd	%xmm1, %xmm4
	subsd	%xmm5, %xmm4
	addsd	%xmm0, %xmm4
	movupd	%xmm2, 24(%rdi)
	movsd	%xmm4, 40(%rdi)
	retq
.Lfunc_end21:
	.size	_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE, .Lfunc_end21-_Z12line_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm6EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end22, nop
	.type	_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$112, %rsp
	.cfi_def_cfa_offset 128
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	movq	%fs:40, %rax
	movq	%rax, 104(%rsp)
	movups	(%rdx), %xmm0
	movups	16(%rdx), %xmm1
	movaps	%xmm1, 80(%rsp)
	movaps	%xmm0, 64(%rsp)
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	32(%rsi), %xmm2
	movups	48(%rsi), %xmm3
	movaps	%xmm3, 48(%rsp)
	movaps	%xmm2, 32(%rsp)
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, (%rsp)
	leaq	64(%rsp), %rsi
	movq	%rsp, %rdx
	callq	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	movq	%fs:40, %rax
	cmpq	104(%rsp), %rax
	jne	.LBB22_2
# %bb.1:
	movq	%rbx, %rax
	addq	$112, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.LBB22_2:
	.cfi_def_cfa_offset 128
	callq	__stack_chk_fail@PLT
.Lfunc_end22:
	.size	_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end22-_Z12plane_galootRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end23, nop
	.type	_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rdx), %xmm3                   # xmm3 = mem[0],zero
	movsd	8(%rdx), %xmm15                 # xmm15 = mem[0],zero
	movsd	(%rsi), %xmm1                   # xmm1 = mem[0],zero
	movsd	%xmm1, -16(%rsp)                # 8-byte Spill
	movsd	8(%rsi), %xmm9                  # xmm9 = mem[0],zero
	movapd	%xmm1, %xmm0
	mulsd	%xmm3, %xmm0
	movapd	%xmm1, %xmm2
	mulsd	%xmm15, %xmm2
	movsd	16(%rdx), %xmm8                 # xmm8 = mem[0],zero
	movapd	%xmm1, %xmm7
	mulsd	%xmm8, %xmm7
	movsd	24(%rdx), %xmm12                # xmm12 = mem[0],zero
	mulsd	%xmm12, %xmm1
	movsd	%xmm1, -32(%rsp)                # 8-byte Spill
	movapd	%xmm3, %xmm4
	mulsd	%xmm9, %xmm4
	subsd	%xmm4, %xmm2
	movapd	%xmm15, %xmm4
	mulsd	%xmm9, %xmm4
	addsd	%xmm0, %xmm4
	movapd	%xmm8, %xmm11
	mulsd	%xmm9, %xmm11
	movsd	%xmm9, -40(%rsp)                # 8-byte Spill
	movapd	%xmm12, %xmm14
	movsd	16(%rsi), %xmm13                # xmm13 = mem[0],zero
	movapd	%xmm3, %xmm5
	mulsd	%xmm13, %xmm5
	subsd	%xmm5, %xmm7
	movapd	%xmm15, %xmm5
	mulsd	%xmm13, %xmm5
	subsd	%xmm5, %xmm11
	movapd	%xmm8, %xmm6
	mulsd	%xmm13, %xmm6
	addsd	%xmm4, %xmm6
	movapd	%xmm12, %xmm1
	movsd	24(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movapd	%xmm3, %xmm5
	mulsd	%xmm4, %xmm5
	addsd	%xmm11, %xmm5
	movapd	%xmm15, %xmm11
	mulsd	%xmm4, %xmm11
	subsd	%xmm11, %xmm7
	movapd	%xmm8, %xmm11
	mulsd	%xmm4, %xmm11
	addsd	%xmm2, %xmm11
	mulsd	%xmm4, %xmm12
	movsd	32(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movsd	%xmm2, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm3, %xmm10
	mulsd	%xmm2, %xmm10
	subsd	%xmm12, %xmm10
	mulsd	%xmm13, %xmm1
	movapd	%xmm15, %xmm0
	mulsd	%xmm2, %xmm0
	addsd	%xmm1, %xmm0
	mulsd	%xmm9, %xmm14
	movapd	%xmm8, %xmm1
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm14
	movsd	40(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movsd	%xmm2, -24(%rsp)                # 8-byte Spill
	movapd	%xmm3, %xmm1
	mulsd	%xmm2, %xmm1
	subsd	%xmm1, %xmm0
	movapd	%xmm15, %xmm9
	mulsd	%xmm2, %xmm9
	addsd	%xmm10, %xmm9
	movapd	%xmm8, %xmm10
	mulsd	%xmm2, %xmm10
	addsd	-32(%rsp), %xmm10               # 8-byte Folded Reload
	movsd	48(%rsi), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm3, %xmm2
	mulsd	%xmm12, %xmm2
	addsd	%xmm14, %xmm2
	movapd	%xmm15, %xmm14
	mulsd	%xmm12, %xmm14
	subsd	%xmm14, %xmm10
	movapd	%xmm8, %xmm14
	mulsd	%xmm12, %xmm14
	addsd	%xmm9, %xmm14
	movsd	56(%rsi), %xmm1                 # xmm1 = mem[0],zero
	mulsd	%xmm1, %xmm3
	addsd	%xmm10, %xmm3
	mulsd	%xmm1, %xmm15
	addsd	%xmm2, %xmm15
	mulsd	%xmm1, %xmm8
	addsd	%xmm0, %xmm8
	movsd	-16(%rsp), %xmm9                # 8-byte Reload
                                        # xmm9 = mem[0],zero
	mulsd	%xmm9, %xmm3
	movsd	-40(%rsp), %xmm2                # 8-byte Reload
                                        # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm15
	addsd	%xmm3, %xmm15
	mulsd	%xmm13, %xmm8
	addsd	%xmm15, %xmm8
	movapd	%xmm9, %xmm0
	movapd	%xmm9, %xmm10
	mulsd	%xmm6, %xmm0
	movapd	%xmm2, %xmm9
	movapd	%xmm2, %xmm3
	mulsd	%xmm6, %xmm9
	movapd	%xmm13, %xmm2
	mulsd	%xmm6, %xmm2
	mulsd	%xmm1, %xmm6
	addsd	%xmm8, %xmm6
	mulsd	%xmm4, %xmm14
	subsd	%xmm14, %xmm6
	movapd	%xmm10, %xmm1
	mulsd	%xmm11, %xmm1
	subsd	%xmm1, %xmm9
	movapd	%xmm3, %xmm1
	mulsd	%xmm11, %xmm1
	addsd	%xmm0, %xmm1
	movapd	%xmm4, %xmm8
	mulsd	%xmm11, %xmm8
	addsd	%xmm2, %xmm8
	mulsd	%xmm12, %xmm11
	subsd	%xmm11, %xmm6
	mulsd	%xmm7, %xmm10
	subsd	%xmm10, %xmm8
	movapd	%xmm13, %xmm0
	mulsd	%xmm7, %xmm0
	addsd	%xmm1, %xmm0
	movapd	%xmm4, %xmm1
	mulsd	%xmm7, %xmm1
	subsd	%xmm1, %xmm9
	mulsd	-24(%rsp), %xmm7                # 8-byte Folded Reload
	addsd	%xmm6, %xmm7
	mulsd	%xmm5, %xmm3
	subsd	%xmm3, %xmm8
	mulsd	%xmm5, %xmm13
	addsd	%xmm9, %xmm13
	mulsd	%xmm5, %xmm4
	addsd	%xmm0, %xmm4
	mulsd	-8(%rsp), %xmm5                 # 8-byte Folded Reload
	subsd	%xmm5, %xmm7
	movsd	%xmm4, (%rdi)
	movsd	%xmm13, 8(%rdi)
	movsd	%xmm8, 16(%rdi)
	movsd	%xmm7, 24(%rdi)
	retq
.Lfunc_end23:
	.size	_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end23-_Z7plane_aRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end24, nop
	.type	_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm11                  # xmm11 = mem[0],zero
	movsd	8(%rsi), %xmm3                  # xmm3 = mem[0],zero
	movapd	%xmm11, %xmm12
	mulsd	%xmm11, %xmm12
	movapd	%xmm3, %xmm0
	mulsd	%xmm3, %xmm0
	movsd	16(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movsd	24(%rsi), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm12, %xmm6
	subsd	%xmm0, %xmm6
	movapd	%xmm11, %xmm2
	mulsd	%xmm3, %xmm2
	movapd	%xmm1, %xmm5
	mulsd	%xmm10, %xmm5
	movapd	%xmm2, %xmm7
	subsd	%xmm5, %xmm7
	movapd	%xmm11, %xmm15
	mulsd	%xmm1, %xmm15
	movapd	%xmm3, %xmm14
	mulsd	%xmm10, %xmm14
	movapd	%xmm15, %xmm4
	addsd	%xmm14, %xmm4
	movsd	%xmm4, -16(%rsp)                # 8-byte Spill
	addsd	%xmm2, %xmm5
	movapd	%xmm0, %xmm9
	subsd	%xmm12, %xmm9
	movapd	%xmm11, %xmm2
	mulsd	%xmm10, %xmm2
	movapd	%xmm3, %xmm8
	mulsd	%xmm1, %xmm8
	movapd	%xmm8, %xmm4
	subsd	%xmm2, %xmm4
	movsd	%xmm4, -8(%rsp)                 # 8-byte Spill
	subsd	%xmm14, %xmm15
	addsd	%xmm2, %xmm8
	addsd	%xmm12, %xmm0
	movsd	56(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movsd	%xmm2, -32(%rsp)                # 8-byte Spill
	movapd	%xmm11, %xmm14
	mulsd	%xmm2, %xmm14
	movsd	48(%rsi), %xmm13                # xmm13 = mem[0],zero
	movapd	%xmm3, %xmm12
	mulsd	%xmm13, %xmm12
	movsd	%xmm13, -24(%rsp)               # 8-byte Spill
	addsd	%xmm14, %xmm12
	movsd	40(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movapd	%xmm1, %xmm14
	mulsd	%xmm4, %xmm14
	subsd	%xmm14, %xmm12
	movsd	32(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movapd	%xmm10, %xmm14
	mulsd	%xmm2, %xmm14
	subsd	%xmm14, %xmm12
	movapd	%xmm11, %xmm14
	mulsd	%xmm13, %xmm14
	movapd	%xmm3, %xmm13
	mulsd	-32(%rsp), %xmm13               # 8-byte Folded Reload
	subsd	%xmm14, %xmm13
	movapd	%xmm1, %xmm14
	mulsd	%xmm2, %xmm14
	addsd	%xmm13, %xmm14
	movapd	%xmm10, %xmm13
	mulsd	%xmm4, %xmm13
	subsd	%xmm13, %xmm14
	mulsd	%xmm4, %xmm11
	mulsd	%xmm2, %xmm3
	subsd	%xmm3, %xmm11
	movapd	%xmm1, %xmm2
	mulsd	-32(%rsp), %xmm1                # 8-byte Folded Reload
	addsd	%xmm11, %xmm1
	movapd	%xmm10, %xmm3
	mulsd	-24(%rsp), %xmm10               # 8-byte Folded Reload
	mulsd	%xmm2, %xmm2
	mulsd	%xmm3, %xmm3
	subsd	%xmm2, %xmm6
	addsd	%xmm3, %xmm6
	addsd	%xmm7, %xmm7
	movsd	-16(%rsp), %xmm4                # 8-byte Reload
                                        # xmm4 = mem[0],zero
	addsd	%xmm4, %xmm4
	subsd	%xmm2, %xmm9
	addsd	%xmm3, %xmm9
	subsd	%xmm10, %xmm1
	movapd	%xmm2, %xmm10
	subsd	%xmm0, %xmm10
	addsd	%xmm3, %xmm10
	addsd	%xmm2, %xmm0
	addsd	%xmm3, %xmm0
	movsd	(%rdx), %xmm11                  # xmm11 = mem[0],zero
	movsd	8(%rdx), %xmm2                  # xmm2 = mem[0],zero
	mulsd	%xmm11, %xmm6
	mulsd	%xmm2, %xmm7
	addsd	%xmm6, %xmm7
	movsd	16(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm4, %xmm3
	mulsd	%xmm6, %xmm3
	addsd	%xmm7, %xmm3
	addsd	%xmm5, %xmm5
	mulsd	%xmm11, %xmm5
	mulsd	%xmm2, %xmm9
	addsd	%xmm5, %xmm9
	movsd	-8(%rsp), %xmm4                 # 8-byte Reload
                                        # xmm4 = mem[0],zero
	addsd	%xmm4, %xmm4
	mulsd	%xmm6, %xmm4
	addsd	%xmm9, %xmm4
	addsd	%xmm15, %xmm15
	addsd	%xmm8, %xmm8
	mulsd	%xmm11, %xmm15
	mulsd	%xmm2, %xmm8
	addsd	%xmm15, %xmm8
	mulsd	%xmm6, %xmm10
	addsd	%xmm8, %xmm10
	addsd	%xmm12, %xmm12
	mulsd	%xmm11, %xmm12
	addsd	%xmm14, %xmm14
	mulsd	%xmm2, %xmm14
	addsd	%xmm12, %xmm14
	addsd	%xmm1, %xmm1
	mulsd	%xmm6, %xmm1
	addsd	%xmm14, %xmm1
	mulsd	24(%rdx), %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm3, (%rdi)
	movsd	%xmm4, 8(%rdi)
	movsd	%xmm10, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end24:
	.size	_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end24-_Z7plane_bRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end25, nop
	.type	_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm5                   # xmm5 = mem[0],zero
	movsd	8(%rsi), %xmm7                  # xmm7 = mem[0],zero
	movapd	%xmm5, %xmm6
	movapd	%xmm5, %xmm9
	mulsd	%xmm7, %xmm9
	movsd	16(%rsi), %xmm0                 # xmm0 = mem[0],zero
	movapd	%xmm5, %xmm1
	mulsd	%xmm0, %xmm1
	movsd	%xmm1, -8(%rsp)                 # 8-byte Spill
	movapd	%xmm5, %xmm8
	movsd	40(%rsi), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm5, %xmm13
	mulsd	%xmm11, %xmm13
	movsd	48(%rsi), %xmm12                # xmm12 = mem[0],zero
	movapd	%xmm5, %xmm10
	mulsd	%xmm12, %xmm10
	movsd	56(%rsi), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm5
	movapd	%xmm7, %xmm1
	movapd	%xmm7, %xmm3
	mulsd	%xmm0, %xmm3
	movsd	32(%rsi), %xmm15                # xmm15 = mem[0],zero
	movapd	%xmm7, %xmm4
	mulsd	%xmm15, %xmm4
	subsd	%xmm4, %xmm13
	movapd	%xmm7, %xmm4
	mulsd	%xmm12, %xmm4
	addsd	%xmm5, %xmm4
	movapd	%xmm7, %xmm14
	mulsd	%xmm2, %xmm7
	subsd	%xmm10, %xmm7
	movapd	%xmm0, %xmm5
	mulsd	%xmm15, %xmm5
	addsd	%xmm7, %xmm5
	movapd	%xmm0, %xmm7
	mulsd	%xmm11, %xmm7
	subsd	%xmm7, %xmm4
	movapd	%xmm0, %xmm10
	movapd	%xmm0, %xmm7
	mulsd	%xmm2, %xmm0
	mulsd	%xmm6, %xmm6
	addsd	%xmm13, %xmm0
	movsd	24(%rsi), %xmm2                 # xmm2 = mem[0],zero
	mulsd	%xmm2, %xmm8
	mulsd	%xmm1, %xmm1
	mulsd	%xmm2, %xmm14
	mulsd	%xmm10, %xmm10
	mulsd	%xmm2, %xmm7
	mulsd	%xmm2, %xmm15
	subsd	%xmm15, %xmm4
	movapd	%xmm2, %xmm15
	mulsd	%xmm2, %xmm11
	subsd	%xmm11, %xmm5
	mulsd	%xmm12, %xmm2
	subsd	%xmm2, %xmm0
	movapd	%xmm6, %xmm11
	subsd	%xmm1, %xmm11
	movapd	%xmm9, %xmm12
	subsd	%xmm7, %xmm12
	movsd	-8(%rsp), %xmm2                 # 8-byte Reload
                                        # xmm2 = mem[0],zero
	movapd	%xmm2, %xmm13
	addsd	%xmm14, %xmm13
	addsd	%xmm9, %xmm7
	movapd	%xmm1, %xmm9
	subsd	%xmm6, %xmm9
	subsd	%xmm14, %xmm2
	movapd	%xmm3, %xmm14
	subsd	%xmm8, %xmm14
	addsd	%xmm8, %xmm3
	movapd	%xmm10, %xmm8
	subsd	%xmm6, %xmm8
	subsd	%xmm1, %xmm8
	addsd	%xmm6, %xmm1
	mulsd	%xmm15, %xmm15
	subsd	%xmm10, %xmm11
	addsd	%xmm15, %xmm11
	subsd	%xmm10, %xmm9
	addsd	%xmm15, %xmm9
	addsd	%xmm15, %xmm8
	addsd	%xmm10, %xmm1
	addsd	%xmm15, %xmm1
	addsd	%xmm12, %xmm12
	addsd	%xmm13, %xmm13
	movsd	(%rdx), %xmm10                  # xmm10 = mem[0],zero
	movsd	8(%rdx), %xmm6                  # xmm6 = mem[0],zero
	mulsd	%xmm10, %xmm11
	mulsd	%xmm6, %xmm12
	addsd	%xmm11, %xmm12
	movsd	16(%rdx), %xmm11                # xmm11 = mem[0],zero
	mulsd	%xmm11, %xmm13
	addsd	%xmm12, %xmm13
	movsd	%xmm13, (%rdi)
	addsd	%xmm7, %xmm7
	mulsd	%xmm10, %xmm7
	mulsd	%xmm6, %xmm9
	addsd	%xmm7, %xmm9
	addsd	%xmm14, %xmm14
	mulsd	%xmm11, %xmm14
	addsd	%xmm9, %xmm14
	movsd	%xmm14, 8(%rdi)
	addsd	%xmm2, %xmm2
	addsd	%xmm3, %xmm3
	mulsd	%xmm10, %xmm2
	mulsd	%xmm6, %xmm3
	addsd	%xmm2, %xmm3
	mulsd	%xmm11, %xmm8
	addsd	%xmm3, %xmm8
	movsd	%xmm8, 16(%rdi)
	addsd	%xmm4, %xmm4
	mulsd	%xmm10, %xmm4
	addsd	%xmm5, %xmm5
	mulsd	%xmm6, %xmm5
	addsd	%xmm4, %xmm5
	addsd	%xmm0, %xmm0
	mulsd	%xmm11, %xmm0
	mulsd	24(%rdx), %xmm1
	addsd	%xmm5, %xmm0
	addsd	%xmm0, %xmm1
	movsd	%xmm1, 24(%rdi)
	retq
.Lfunc_end25:
	.size	_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end25-_Z8plane_bpRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE # -- Begin function _Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end26, nop
	.type	_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE,@function
_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE: # @_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm0                   # xmm0 = mem[0],zero
	movsd	8(%rsi), %xmm9                  # xmm9 = mem[0],zero
	movapd	%xmm0, %xmm3
	addsd	%xmm0, %xmm3
	movapd	%xmm9, %xmm8
	addsd	%xmm9, %xmm8
	movsd	16(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movapd	%xmm1, %xmm7
	addsd	%xmm1, %xmm7
	movsd	24(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movapd	%xmm2, %xmm5
	addsd	%xmm2, %xmm5
	movsd	48(%rsi), %xmm10                # xmm10 = mem[0],zero
	movapd	%xmm9, %xmm11
	mulsd	%xmm10, %xmm11
	movsd	40(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movapd	%xmm1, %xmm6
	mulsd	%xmm4, %xmm6
	subsd	%xmm6, %xmm11
	movsd	32(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm1, %xmm12
	mulsd	%xmm6, %xmm12
	movapd	%xmm0, %xmm13
	mulsd	%xmm10, %xmm13
	subsd	%xmm13, %xmm12
	movapd	%xmm0, %xmm13
	mulsd	%xmm4, %xmm13
	movapd	%xmm9, %xmm14
	mulsd	%xmm6, %xmm14
	subsd	%xmm14, %xmm13
	mulsd	%xmm2, %xmm6
	movsd	56(%rsi), %xmm14                # xmm14 = mem[0],zero
	movapd	%xmm0, %xmm15
	mulsd	%xmm14, %xmm15
	subsd	%xmm15, %xmm6
	addsd	%xmm11, %xmm6
	mulsd	%xmm2, %xmm4
	movapd	%xmm9, %xmm11
	mulsd	%xmm14, %xmm11
	subsd	%xmm11, %xmm4
	addsd	%xmm12, %xmm4
	mulsd	%xmm10, %xmm2
	mulsd	%xmm1, %xmm14
	subsd	%xmm14, %xmm2
	addsd	%xmm13, %xmm2
	addsd	%xmm6, %xmm6
	addsd	%xmm4, %xmm4
	addsd	%xmm2, %xmm2
	movsd	16(%rdx), %xmm11                # xmm11 = mem[0],zero
	movapd	%xmm9, %xmm10
	mulsd	%xmm11, %xmm10
	movsd	(%rdx), %xmm13                  # xmm13 = mem[0],zero
	movsd	8(%rdx), %xmm12                 # xmm12 = mem[0],zero
	movapd	%xmm1, %xmm14
	mulsd	%xmm12, %xmm14
	subsd	%xmm14, %xmm10
	mulsd	%xmm13, %xmm1
	movapd	%xmm0, %xmm14
	mulsd	%xmm11, %xmm14
	subsd	%xmm14, %xmm1
	mulsd	%xmm12, %xmm0
	mulsd	%xmm13, %xmm9
	subsd	%xmm9, %xmm0
	movapd	%xmm8, %xmm9
	mulsd	%xmm0, %xmm9
	movapd	%xmm7, %xmm14
	mulsd	%xmm1, %xmm14
	subsd	%xmm14, %xmm9
	mulsd	%xmm10, %xmm7
	movapd	%xmm3, %xmm14
	mulsd	%xmm0, %xmm14
	subsd	%xmm14, %xmm7
	mulsd	%xmm1, %xmm3
	mulsd	%xmm10, %xmm8
	subsd	%xmm8, %xmm3
	mulsd	%xmm5, %xmm10
	addsd	%xmm9, %xmm10
	addsd	%xmm13, %xmm10
	mulsd	%xmm5, %xmm1
	addsd	%xmm7, %xmm1
	addsd	%xmm12, %xmm1
	mulsd	%xmm5, %xmm0
	addsd	%xmm3, %xmm0
	addsd	%xmm11, %xmm0
	mulsd	%xmm10, %xmm6
	mulsd	%xmm1, %xmm4
	addsd	%xmm6, %xmm4
	mulsd	%xmm0, %xmm2
	addsd	%xmm4, %xmm2
	movsd	24(%rdx), %xmm3                 # xmm3 = mem[0],zero
	subsd	%xmm2, %xmm3
	movsd	%xmm10, (%rdi)
	movsd	%xmm1, 8(%rdi)
	movsd	%xmm0, 16(%rdi)
	movsd	%xmm3, 24(%rdi)
	retq
.Lfunc_end26:
	.size	_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE, .Lfunc_end26-_Z7plane_cRKSt5arrayIdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          # -- Begin function _Z13plane_b_setupRKSt5arrayIdLm8EE
.LCPI27_0:
	.quad	0x3ff0000000000000              # double 1
	.text
	.globl	_Z13plane_b_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end27, nop
	.type	_Z13plane_b_setupRKSt5arrayIdLm8EE,@function
_Z13plane_b_setupRKSt5arrayIdLm8EE:     # @_Z13plane_b_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm0                   # xmm0 = mem[0],zero
	movsd	8(%rsi), %xmm4                  # xmm4 = mem[0],zero
	movsd	16(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm0, %xmm8
	mulsd	%xmm0, %xmm8
	movapd	%xmm4, %xmm3
	mulsd	%xmm4, %xmm3
	movapd	%xmm0, %xmm12
	mulsd	%xmm4, %xmm12
	movapd	%xmm8, %xmm7
	subsd	%xmm3, %xmm7
	movapd	%xmm0, %xmm2
	mulsd	%xmm6, %xmm2
	movapd	%xmm3, %xmm5
	subsd	%xmm8, %xmm5
	movapd	%xmm4, %xmm15
	mulsd	%xmm6, %xmm15
	movapd	%xmm6, %xmm9
	mulsd	%xmm6, %xmm9
	subsd	%xmm9, %xmm7
	subsd	%xmm9, %xmm5
	addsd	%xmm8, %xmm3
	movapd	%xmm9, %xmm8
	subsd	%xmm3, %xmm8
	movsd	%xmm8, -16(%rsp)                # 8-byte Spill
	movapd	%xmm0, %xmm1
	addsd	%xmm9, %xmm3
	movupd	24(%rsi), %xmm9
	movapd	%xmm6, %xmm10
	mulsd	%xmm9, %xmm10
	movapd	%xmm12, %xmm11
	addsd	%xmm10, %xmm11
	movapd	%xmm4, %xmm13
	mulsd	%xmm9, %xmm13
	movapd	%xmm2, %xmm8
	addsd	%xmm13, %xmm8
	unpcklpd	%xmm11, %xmm8                   # xmm8 = xmm8[0],xmm11[0]
	movapd	%xmm9, %xmm14
	mulsd	%xmm9, %xmm14
	subsd	%xmm10, %xmm12
	movapd	%xmm14, %xmm10
	unpcklpd	%xmm12, %xmm10                  # xmm10 = xmm10[0],xmm12[0]
	unpcklpd	%xmm12, %xmm7                   # xmm7 = xmm7[0],xmm12[0]
	movupd	40(%rsi), %xmm12
	addpd	%xmm10, %xmm7
	movapd	%xmm0, %xmm10
	mulsd	%xmm9, %xmm10
	subsd	%xmm13, %xmm2
	movapd	%xmm15, %xmm11
	addsd	%xmm10, %xmm11
	unpcklpd	%xmm11, %xmm2                   # xmm2 = xmm2[0],xmm11[0]
	movsd	48(%rsi), %xmm11                # xmm11 = mem[0],zero
	subsd	%xmm10, %xmm15
	movapd	%xmm4, %xmm13
	mulsd	%xmm11, %xmm13
	mulsd	%xmm0, %xmm11
	movsd	%xmm11, -8(%rsp)                # 8-byte Spill
	mulsd	%xmm12, %xmm0
	movapd	%xmm4, %xmm11
	movsd	32(%rsi), %xmm10                # xmm10 = mem[0],zero
	mulsd	%xmm10, %xmm4
	subsd	%xmm4, %xmm0
	movsd	56(%rsi), %xmm4                 # xmm4 = mem[0],zero
	mulsd	%xmm4, %xmm1
	mulsd	%xmm4, %xmm11
	mulsd	%xmm6, %xmm4
	addsd	%xmm4, %xmm0
	movsd	-16(%rsp), %xmm4                # 8-byte Reload
                                        # xmm4 = mem[0],zero
	addsd	%xmm14, %xmm4
	movsd	%xmm4, -16(%rsp)                # 8-byte Spill
	unpcklpd	%xmm14, %xmm0                   # xmm0 = xmm0[0],xmm14[0]
	unpcklpd	%xmm15, %xmm14                  # xmm14 = xmm14[0],xmm15[0]
	unpcklpd	%xmm15, %xmm5                   # xmm5 = xmm5[0],xmm15[0]
	addpd	%xmm14, %xmm5
	addsd	%xmm1, %xmm13
	movapd	%xmm6, %xmm4
	mulsd	%xmm12, %xmm4
	subsd	%xmm4, %xmm13
	subsd	-8(%rsp), %xmm11                # 8-byte Folded Reload
	mulsd	%xmm10, %xmm6
	addsd	%xmm11, %xmm6
	unpcklpd	%xmm6, %xmm13                   # xmm13 = xmm13[0],xmm6[0]
	movapd	%xmm9, %xmm4
	unpcklpd	%xmm9, %xmm4                    # xmm4 = xmm4[0],xmm9[0]
	movq	%xmm9, %xmm6                    # xmm6 = xmm9[0],zero
	shufpd	$1, %xmm12, %xmm9               # xmm9 = xmm9[1],xmm12[0]
	mulpd	%xmm4, %xmm9
	subpd	%xmm9, %xmm13
	movsd	.LCPI27_0(%rip), %xmm4          # xmm4 = [1.0E+0,0.0E+0]
	shufpd	$1, %xmm4, %xmm12               # xmm12 = xmm12[1],xmm4[0]
	mulpd	%xmm6, %xmm12
	subpd	%xmm12, %xmm0
	movapd	%xmm0, %xmm4
	unpcklpd	%xmm3, %xmm4                    # xmm4 = xmm4[0],xmm3[0]
	addpd	%xmm0, %xmm4
	movupd	%xmm7, (%rdi)
	addpd	%xmm8, %xmm8
	movupd	%xmm8, 16(%rdi)
	movupd	%xmm5, 32(%rdi)
	addpd	%xmm2, %xmm2
	movupd	%xmm2, 48(%rdi)
	movsd	-16(%rsp), %xmm0                # 8-byte Reload
                                        # xmm0 = mem[0],zero
	movsd	%xmm0, 64(%rdi)
	addpd	%xmm13, %xmm13
	movupd	%xmm13, 72(%rdi)
	movupd	%xmm4, 88(%rdi)
	retq
.Lfunc_end27:
	.size	_Z13plane_b_setupRKSt5arrayIdLm8EE, .Lfunc_end27-_Z13plane_b_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE # -- Begin function _Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end28, nop
	.type	_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE,@function
_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE: # @_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movupd	(%rcx), %xmm2
	movupd	16(%rcx), %xmm0
	movapd	%xmm2, %xmm4
	shufpd	$1, %xmm2, %xmm4                # xmm4 = xmm4[1],xmm2[0]
	movupd	(%rsi), %xmm3
	movupd	16(%rsi), %xmm6
	movupd	32(%rsi), %xmm5
	movupd	48(%rsi), %xmm1
	movapd	%xmm3, %xmm7
	unpckhpd	%xmm6, %xmm7                    # xmm7 = xmm7[1],xmm6[1]
	mulpd	%xmm4, %xmm7
	unpcklpd	%xmm5, %xmm3                    # xmm3 = xmm3[0],xmm5[0]
	mulpd	%xmm2, %xmm3
	addpd	%xmm7, %xmm3
	movsd	%xmm6, %xmm5                    # xmm5 = xmm6[0],xmm5[1]
	movapd	%xmm2, %xmm4
	unpckhpd	%xmm2, %xmm4                    # xmm4 = xmm4[1],xmm2[1]
	movupd	72(%rsi), %xmm6
	mulpd	%xmm2, %xmm6
	movupd	88(%rsi), %xmm7
	unpcklpd	%xmm7, %xmm1                    # xmm1 = xmm1[0],xmm7[0]
	movlpd	64(%rsi), %xmm7                 # xmm7 = mem[0],xmm7[1]
	mulpd	%xmm0, %xmm7
	unpcklpd	%xmm0, %xmm2                    # xmm2 = xmm2[0],xmm0[0]
	unpcklpd	%xmm0, %xmm0                    # xmm0 = xmm0[0,0]
	mulpd	%xmm5, %xmm0
	addpd	%xmm3, %xmm0
	mulpd	%xmm2, %xmm1
	movapd	%xmm6, %xmm2
	unpckhpd	%xmm6, %xmm2                    # xmm2 = xmm2[1],xmm6[1]
	addpd	%xmm6, %xmm2
	mulsd	56(%rsi), %xmm4
	unpcklpd	%xmm2, %xmm4                    # xmm4 = xmm4[0],xmm2[0]
	addpd	%xmm1, %xmm4
	addpd	%xmm7, %xmm4
	movupd	%xmm0, (%rdi)
	movupd	%xmm4, 16(%rdi)
	retq
.Lfunc_end28:
	.size	_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE, .Lfunc_end28-_Z13plane_b_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          # -- Begin function _Z14plane_bp_setupRKSt5arrayIdLm8EE
.LCPI29_0:
	.quad	0x3ff0000000000000              # double 1
	.text
	.globl	_Z14plane_bp_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end29, nop
	.type	_Z14plane_bp_setupRKSt5arrayIdLm8EE,@function
_Z14plane_bp_setupRKSt5arrayIdLm8EE:    # @_Z14plane_bp_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	56(%rsi), %xmm6                 # xmm6 = mem[0],zero
	movupd	24(%rsi), %xmm10
	movupd	40(%rsi), %xmm0
	movapd	%xmm0, -24(%rsp)                # 16-byte Spill
	movsd	48(%rsi), %xmm8                 # xmm8 = mem[0],zero
	movsd	32(%rsi), %xmm9                 # xmm9 = mem[0],zero
	movsd	(%rsi), %xmm1                   # xmm1 = mem[0],zero
	movsd	8(%rsi), %xmm2                  # xmm2 = mem[0],zero
	movsd	16(%rsi), %xmm4                 # xmm4 = mem[0],zero
	movapd	%xmm1, %xmm7
	mulsd	%xmm0, %xmm7
	movapd	%xmm1, %xmm0
	mulsd	%xmm8, %xmm0
	movapd	%xmm1, %xmm3
	mulsd	%xmm6, %xmm3
	movapd	%xmm2, %xmm5
	mulsd	%xmm9, %xmm5
	subsd	%xmm5, %xmm7
	movsd	%xmm7, -48(%rsp)                # 8-byte Spill
	mulsd	%xmm2, %xmm8
	addsd	%xmm3, %xmm8
	movapd	%xmm2, %xmm14
	mulsd	%xmm6, %xmm14
	subsd	%xmm0, %xmm14
	movapd	%xmm4, %xmm5
	mulsd	%xmm10, %xmm5
	movapd	%xmm10, %xmm3
	xorpd	%xmm0, %xmm0
	movapd	%xmm10, %xmm7
	movlpd	.LCPI29_0(%rip), %xmm7          # xmm7 = mem[0],xmm7[1]
	unpcklpd	%xmm10, %xmm0                   # xmm0 = xmm0[0],xmm10[0]
	mulpd	%xmm0, %xmm7
	movapd	%xmm7, -40(%rsp)                # 16-byte Spill
	movapd	%xmm2, %xmm0
	mulsd	%xmm10, %xmm0
	movapd	%xmm1, %xmm12
	mulsd	%xmm2, %xmm12
	movapd	%xmm12, %xmm11
	addsd	%xmm5, %xmm11
	movapd	%xmm10, %xmm15
	mulsd	%xmm1, %xmm10
	movapd	%xmm1, %xmm7
	mulsd	%xmm4, %xmm1
	movapd	%xmm1, %xmm13
	addsd	%xmm0, %xmm13
	unpcklpd	%xmm11, %xmm13                  # xmm13 = xmm13[0],xmm11[0]
	subsd	%xmm5, %xmm12
	subsd	%xmm0, %xmm1
	movapd	%xmm2, %xmm0
	mulsd	%xmm4, %xmm2
	movapd	%xmm2, %xmm5
	addsd	%xmm10, %xmm5
	unpcklpd	%xmm5, %xmm1                    # xmm1 = xmm1[0],xmm5[0]
	subsd	%xmm10, %xmm2
	mulsd	%xmm4, %xmm6
	addsd	-48(%rsp), %xmm6                # 8-byte Folded Reload
	mulsd	%xmm4, %xmm9
	addsd	%xmm14, %xmm9
	unpcklpd	%xmm6, %xmm9                    # xmm9 = xmm9[0],xmm6[0]
	mulsd	%xmm3, %xmm3
	unpcklpd	%xmm15, %xmm15                  # xmm15 = xmm15[0,0]
	movapd	-24(%rsp), %xmm5                # 16-byte Reload
	mulpd	%xmm5, %xmm15
	mulsd	%xmm0, %xmm0
	mulsd	%xmm4, %xmm5
	mulsd	%xmm7, %xmm7
	subpd	%xmm15, %xmm9
	movapd	%xmm7, %xmm10
	subsd	%xmm0, %xmm10
	subsd	%xmm5, %xmm8
	movapd	%xmm3, %xmm6
	unpcklpd	%xmm8, %xmm6                    # xmm6 = xmm6[0],xmm8[0]
	movapd	%xmm0, %xmm5
	subsd	%xmm7, %xmm5
	mulsd	%xmm4, %xmm4
	subpd	-40(%rsp), %xmm6                # 16-byte Folded Reload
	movapd	%xmm4, %xmm8
	subsd	%xmm7, %xmm8
	subsd	%xmm0, %xmm8
	addsd	%xmm0, %xmm7
	subsd	%xmm4, %xmm10
	subsd	%xmm4, %xmm5
	addsd	%xmm4, %xmm7
	unpcklpd	%xmm12, %xmm10                  # xmm10 = xmm10[0],xmm12[0]
	movapd	%xmm3, %xmm0
	unpcklpd	%xmm12, %xmm0                   # xmm0 = xmm0[0],xmm12[0]
	addpd	%xmm10, %xmm0
	movupd	%xmm0, (%rdi)
	addpd	%xmm13, %xmm13
	movupd	%xmm13, 16(%rdi)
	addsd	%xmm3, %xmm7
	unpcklpd	%xmm2, %xmm5                    # xmm5 = xmm5[0],xmm2[0]
	unpcklpd	%xmm2, %xmm3                    # xmm3 = xmm3[0],xmm2[0]
	addpd	%xmm5, %xmm3
	movupd	%xmm3, 32(%rdi)
	addpd	%xmm1, %xmm1
	movupd	%xmm1, 48(%rdi)
	shufpd	$2, %xmm6, %xmm8                # xmm8 = xmm8[0],xmm6[1]
	addpd	%xmm6, %xmm8
	movupd	%xmm8, 64(%rdi)
	addpd	%xmm9, %xmm9
	movupd	%xmm9, 80(%rdi)
	movsd	%xmm7, 96(%rdi)
	retq
.Lfunc_end29:
	.size	_Z14plane_bp_setupRKSt5arrayIdLm8EE, .Lfunc_end29-_Z14plane_bp_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE # -- Begin function _Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end30, nop
	.type	_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE,@function
_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE: # @_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movupd	(%rsi), %xmm2
	movupd	16(%rsi), %xmm3
	movupd	32(%rsi), %xmm4
	movupd	(%rcx), %xmm1
	movupd	16(%rcx), %xmm0
	movapd	%xmm1, %xmm5
	shufpd	$1, %xmm1, %xmm5                # xmm5 = xmm5[1],xmm1[0]
	movapd	%xmm2, %xmm6
	unpckhpd	%xmm3, %xmm6                    # xmm6 = xmm6[1],xmm3[1]
	mulpd	%xmm5, %xmm6
	unpcklpd	%xmm4, %xmm2                    # xmm2 = xmm2[0],xmm4[0]
	mulpd	%xmm1, %xmm2
	addpd	%xmm6, %xmm2
	movsd	%xmm3, %xmm4                    # xmm4 = xmm3[0],xmm4[1]
	movapd	%xmm1, %xmm3
	unpckhpd	%xmm1, %xmm3                    # xmm3 = xmm3[1],xmm1[1]
	movupd	72(%rsi), %xmm5
	mulpd	%xmm1, %xmm5
	movapd	%xmm0, %xmm6
	unpcklpd	%xmm0, %xmm6                    # xmm6 = xmm6[0],xmm0[0]
	mulpd	%xmm4, %xmm6
	addpd	%xmm2, %xmm6
	movsd	64(%rsi), %xmm2                 # xmm2 = mem[0],zero
	movupd	%xmm6, (%rdi)
	movapd	%xmm5, %xmm4
	unpckhpd	%xmm5, %xmm4                    # xmm4 = xmm4[1],xmm5[1]
	addpd	%xmm5, %xmm4
	mulsd	56(%rsi), %xmm3
	unpcklpd	%xmm4, %xmm3                    # xmm3 = xmm3[0],xmm4[0]
	movupd	48(%rsi), %xmm4
	movupd	88(%rsi), %xmm5
	unpcklpd	%xmm0, %xmm1                    # xmm1 = xmm1[0],xmm0[0]
	unpcklpd	%xmm5, %xmm4                    # xmm4 = xmm4[0],xmm5[0]
	mulpd	%xmm1, %xmm4
	addpd	%xmm3, %xmm4
	shufpd	$2, %xmm5, %xmm2                # xmm2 = xmm2[0],xmm5[1]
	mulpd	%xmm0, %xmm2
	addpd	%xmm4, %xmm2
	movupd	%xmm2, 16(%rdi)
	retq
.Lfunc_end30:
	.size	_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE, .Lfunc_end30-_Z14plane_bp_applyRKSt5arrayIdLm13EERKS_IdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13plane_c_setupRKSt5arrayIdLm8EE # -- Begin function _Z13plane_c_setupRKSt5arrayIdLm8EE
	.prefalign	4, .Lfunc_end31, nop
	.type	_Z13plane_c_setupRKSt5arrayIdLm8EE,@function
_Z13plane_c_setupRKSt5arrayIdLm8EE:     # @_Z13plane_c_setupRKSt5arrayIdLm8EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	56(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movupd	(%rsi), %xmm3
	movapd	%xmm3, %xmm0
	addpd	%xmm3, %xmm0
	movupd	%xmm0, (%rdi)
	movupd	16(%rsi), %xmm0
	movapd	%xmm0, %xmm4
	addpd	%xmm0, %xmm4
	movapd	%xmm0, %xmm2
	mulsd	%xmm1, %xmm2
	movupd	%xmm4, 16(%rdi)
	movupd	32(%rsi), %xmm5
	movupd	40(%rsi), %xmm6
	movapd	%xmm3, %xmm7
	unpckhpd	%xmm3, %xmm7                    # xmm7 = xmm7[1],xmm3[1]
	mulsd	%xmm5, %xmm7
	movapd	%xmm3, %xmm4
	mulsd	%xmm6, %xmm4
	subsd	%xmm7, %xmm4
	unpcklpd	%xmm1, %xmm1                    # xmm1 = xmm1[0,0]
	mulpd	%xmm3, %xmm1
	movapd	%xmm0, %xmm7
	unpcklpd	%xmm3, %xmm7                    # xmm7 = xmm7[0],xmm3[0]
	mulpd	%xmm6, %xmm7
	shufpd	$1, %xmm0, %xmm3                # xmm3 = xmm3[1],xmm0[0]
	movapd	%xmm0, %xmm8
	mulpd	%xmm6, %xmm0
	shufpd	$1, %xmm5, %xmm6                # xmm6 = xmm6[1],xmm5[0]
	mulpd	%xmm3, %xmm6
	subpd	%xmm7, %xmm6
	unpckhpd	%xmm8, %xmm8                    # xmm8 = xmm8[1,1]
	mulpd	%xmm5, %xmm8
	subpd	%xmm1, %xmm8
	addpd	%xmm6, %xmm8
	unpckhpd	%xmm0, %xmm0                    # xmm0 = xmm0[1,1]
	subsd	%xmm2, %xmm0
	addsd	%xmm4, %xmm0
	addpd	%xmm8, %xmm8
	addsd	%xmm0, %xmm0
	movupd	%xmm8, 32(%rdi)
	movsd	%xmm0, 48(%rdi)
	retq
.Lfunc_end31:
	.size	_Z13plane_c_setupRKSt5arrayIdLm8EE, .Lfunc_end31-_Z13plane_c_setupRKSt5arrayIdLm8EE
	.cfi_endproc
                                        # -- End function
	.globl	_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE # -- Begin function _Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE
	.prefalign	4, .Lfunc_end32, nop
	.type	_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE,@function
_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE: # @_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	(%rsi), %xmm3                   # xmm3 = mem[0],zero
	movsd	24(%rsi), %xmm1                 # xmm1 = mem[0],zero
	movsd	24(%rcx), %xmm0                 # xmm0 = mem[0],zero
	movupd	(%rdx), %xmm6
	movupd	8(%rdx), %xmm7
	movupd	(%rcx), %xmm4
	movupd	8(%rcx), %xmm8
	movapd	%xmm6, %xmm2
	mulpd	%xmm8, %xmm2
	movapd	%xmm7, %xmm5
	shufpd	$1, %xmm6, %xmm5                # xmm5 = xmm5[1],xmm6[0]
	mulpd	%xmm4, %xmm5
	mulpd	%xmm4, %xmm7
	subpd	%xmm7, %xmm2
	shufpd	$1, %xmm4, %xmm8                # xmm8 = xmm8[1],xmm4[0]
	mulpd	%xmm6, %xmm8
	subpd	%xmm8, %xmm5
	movupd	8(%rsi), %xmm6
	movapd	%xmm6, %xmm7
	movapd	%xmm2, %xmm8
	unpckhpd	%xmm2, %xmm8                    # xmm8 = xmm8[1],xmm2[1]
	mulsd	%xmm6, %xmm8
	mulpd	%xmm2, %xmm6
	shufpd	$1, %xmm3, %xmm7                # xmm7 = xmm7[1],xmm3[0]
	mulpd	%xmm5, %xmm7
	subpd	%xmm7, %xmm6
	mulsd	%xmm5, %xmm3
	subsd	%xmm8, %xmm3
	movapd	%xmm2, %xmm7
	shufpd	$1, %xmm5, %xmm7                # xmm7 = xmm7[1],xmm5[0]
	mulsd	%xmm1, %xmm2
	unpcklpd	%xmm1, %xmm1                    # xmm1 = xmm1[0,0]
	mulpd	%xmm7, %xmm1
	addpd	%xmm6, %xmm1
	addsd	%xmm3, %xmm2
	addsd	16(%rcx), %xmm2
	addpd	%xmm4, %xmm1
	movsd	32(%rsi), %xmm3                 # xmm3 = mem[0],zero
	mulsd	%xmm1, %xmm3
	movapd	%xmm1, %xmm4
	unpckhpd	%xmm1, %xmm4                    # xmm4 = xmm4[1],xmm1[1]
	mulsd	40(%rsi), %xmm4
	addsd	%xmm3, %xmm4
	movsd	48(%rsi), %xmm3                 # xmm3 = mem[0],zero
	mulsd	%xmm2, %xmm3
	addsd	%xmm4, %xmm3
	subsd	%xmm3, %xmm0
	movupd	%xmm1, (%rdi)
	movsd	%xmm2, 16(%rdi)
	movsd	%xmm0, 24(%rdi)
	retq
.Lfunc_end32:
	.size	_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE, .Lfunc_end32-_Z13plane_c_applyRKSt5arrayIdLm7EERKS_IdLm8EERKS_IdLm4EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI33_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end33, nop
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
	movapd	.LCPI33_0(%rip), %xmm2          # xmm2 = [-0.0E+0,-0.0E+0]
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
.Lfunc_end33:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end33-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj1ELi1EEtlS8_Lj2ELi1EEtlS8_Lj4ELi1EEtlS8_Lj8ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI34_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end34, nop
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
	movapd	.LCPI34_0(%rip), %xmm5          # xmm5 = [-0.0E+0,-0.0E+0]
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
	jne	.LBB34_2
# %bb.1:
	movq	%rbx, %rax
	addq	$72, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	retq
.LBB34_2:
	.cfi_def_cfa_offset 96
	callq	__stack_chk_fail@PLT
.Lfunc_end34:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end34-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm6EEEtlSt5arrayINS0_14oriented_bladeELm6EEtlA6_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
.LCPI35_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,"axG",@progbits,_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,comdat
	.weak	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.prefalign	4, .Lfunc_end35, nop
	.type	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE,@function
_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE: # @_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_startproc
# %bb.0:
	movq	%rdi, %rax
	movsd	24(%rdx), %xmm6                 # xmm6 = mem[0],zero
	movapd	%xmm6, %xmm1
	xorpd	.LCPI35_0(%rip), %xmm1
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
	movapd	.LCPI35_0(%rip), %xmm14         # xmm14 = [-0.0E+0,-0.0E+0]
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
.Lfunc_end35:
	.size	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE, .Lfunc_end35-_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelINS_7presets4rga37algebraEXtlNS0_9blade_setILm8EEEtlSt5arrayINS0_14oriented_bladeELm8EEtlA8_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj15ELi1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEtlSC_Lj0ELi1EEEEEEXtlNS9_ILm6EEEtlSB_ISC_Lm6EEtlA6_SC_tlSC_Lj9ELin1EEtlSC_Lj10ELin1EEtlSC_Lj12ELin1EEtlSC_Lj6ELi1EEtlSC_Lj5ELin1EEtlSC_Lj3ELi1EEEEEELb0EEEELm8EdLm8ELm6EJLm0ELm1ELm2ELm3ELm4ELm5ELm6ELm7ELm8ELm9ELm10ELm11ELm12ELm13ELm14ELm15ELm16ELm17ELm18ELm19ELm20ELm21ELm22ELm23ELm24ELm25ELm26ELm27ELm28ELm29ELm30ELm31ELm32ELm33ELm34ELm35EEEESB_IT1_XT0_EERKSB_ISI_XT2_EERKSB_ISI_XT3_EESt16integer_sequenceImJXspT4_EEE
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
.LCPI36_0:
	.quad	0x8000000000000000              # double -0
	.quad	0x8000000000000000              # double -0
	.section	.text._ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,"axG",@progbits,_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE,comdat
	.weak	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.prefalign	4, .Lfunc_end36, nop
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
	xorpd	.LCPI36_0(%rip), %xmm8
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
	movapd	.LCPI36_0(%rip), %xmm6          # xmm6 = [-0.0E+0,-0.0E+0]
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
.Lfunc_end36:
	.size	_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE, .Lfunc_end36-_ZN6galoot4core9transformITkNS0_11metric_likeENS_7presets4rga37algebraETnNS0_9blade_setEXtlNS5_ILm4EEEtlSt5arrayINS0_14oriented_bladeELm4EEtlA4_S8_tlS8_Lj14ELi1EEtlS8_Lj13ELin1EEtlS8_Lj11ELi1EEtlS8_Lj7ELin1EEEEEETnS5_XtlNS5_ILm8EEEtlS7_IS8_Lm8EEtlA8_S8_tlS8_Lj9ELin1EEtlS8_Lj10ELin1EEtlS8_Lj12ELin1EEtlS8_Lj15ELi1EEtlS8_Lj6ELi1EEtlS8_Lj5ELin1EEtlS8_Lj3ELi1EEtlS8_Lj0ELi1EEEEEETkNS0_11ring_scalarEdEENS0_2mvIT_XT0_ET2_EERKSH_RKNSE_ISF_XT1_ESG_EE
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 23.1.4"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __stack_chk_fail
