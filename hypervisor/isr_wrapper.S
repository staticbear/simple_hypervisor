
.intel_syntax noprefix
.globl   isr_wrapper
.globl   get_isr_addr
.align   4
 
get_isr_addr:
    call    m
m:
    pop     rax
    add     rax, 6
    ret
     
isr_wrapper:
    call    common_handler
    jmp     $                
    