	.file	"have_mmx.S"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 16
.globl pixops_have_mmx
	.type	 pixops_have_mmx,@function

pixops_have_mmx:
	push	%ebx

	pushfl	
	popl	%eax
	movl	%eax,%ebx
	xorl	$0x00200000, %eax
	pushl   %eax
	popfl
	pushfl
	popl	%eax

	cmpl	%eax, %ebx

	je .notfound

	movl	$1, %eax
	cpuid
	
	test	$0x00800000, %edx
	jz	.notfound

	movl	$1, %eax
	jmp	.out

.notfound:
	movl  	$0, %eax
.out:	
	popl	%ebx
	ret
