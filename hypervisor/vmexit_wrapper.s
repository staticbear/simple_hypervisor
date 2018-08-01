#include "memory_map.h"

.intel_syntax noprefix
.globl   vmexit_wrapper
.globl   get_vmexit_addr
.align   4
 
get_vmexit_addr:
	call	m
m:
	pop		rax
	add		rax, 6
	ret
	 
vmexit_wrapper:
	push	rax
	mov		rax, GUEST_REGS_addr
	mov		[rax + 1 * 8], rcx
	mov		[rax + 2 * 8], rdx
	mov		[rax + 3 * 8], rbx
	mov		[rax + 5 * 8], rbp
	mov		[rax + 6 * 8], rsi
	mov		[rax + 7 * 8], rdi
	pop		rcx
	mov		[rax + 0 * 8], rcx
	
    call	VMEXIT_handler
	
	mov		rax, GUEST_REGS_addr
	mov		rcx, [rax + 1 * 8] 
	mov		rdx, [rax + 2 * 8] 
	mov		rbx, [rax + 3 * 8] 
	mov		rbp, [rax + 5 * 8] 
	mov		rsi, [rax + 6 * 8]
	mov		rdi, [rax + 7 * 8]
	mov		rax, [rax + 0 * 8]
	
	vmresume

    call	VMEnter_error
	
	