# A call to a function template of the library, as Clang writes it.
	.text
_Z5firstRKd:
	callq	_ZN6galoot4core6detail11run_productITnRKDaL_ZNS1_12first_kernelEELm8EdEESt5arrayIT1_XT0_EE
	ret
_Z6secondv:
	ret
	movq	%rdi, %rax
