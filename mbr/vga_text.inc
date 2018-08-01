;------------------------------------------------------
set_text_mode:
    mov   ax,3
    int   10h
    ret
;------------------------------------------------------
;input ds:si  addr of string
;output:nothing
print_str:
    push  bx
@@:
    lodsb
    test  al,al
    jz    @f
    mov   bx,7
    mov   ah,0Eh
    int   10h
    jmp   @b
@@:
    pop   bx
    ret