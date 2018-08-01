;http://wiki.osdev.org/Serial_Ports

COM1   equ     0x3f8

;------------------------------------------------------

Init_Com_Port64:
    push   rax
    push   rdx

    mov    al, 0x00
    mov    dx, COM1 + 1
    out    dx, al       ; Disable all interrupts

    mov    al, 0x80
    mov    dx, COM1 + 3
    out    dx, al       ; Enable DLAB (set baud rate divisor)

    mov    al, 14
    mov    dx, COM1 + 0
    out    dx, al       ; Set divisor to 14 (lo byte) (8000 baud equ 9600 baud in my PC, I don't know why)

    mov    al, 0x00
    mov    dx, COM1 + 1
    out    dx, al       ; (hi byte)

    mov    al, 0x03
    mov    dx, COM1 + 3
    out    dx, al       ; 8 bits, no parity, one stop bit

    mov    al, 0xC7
    mov    dx, COM1 + 2
    out    dx, al       ; Enable FIFO, clear them, with 14-byte threshold

    mov    al, 0x03
    mov    dx, COM1 + 4
    out    dx, al       ; IRQs enabled, RTS/DSR set

    pop    rdx
    pop    rax
    ret

;------------------------------------------------------
is_transmit_empty64:
    push   rax
    push   rdx

    mov    dx, COM1 + 5
.wait:
    in     al, dx
    test   al, 0x20
    jz     .wait

    pop    rdx
    pop    rax
    ret
;------------------------------------------------------

;arg1 - addr of str (0 - end)
SerialPrintStr64:
    push   rdx
    push   rsi
    mov    rsi, [rsp + 3 * 8]   ;arg 1
.next_byte:

    call   is_transmit_empty64
    lodsb

    mov    dx, COM1
    out    dx, al
    test   al, al
    jnz    .next_byte

    call   is_transmit_empty64
    mov    dx, COM1
    mov    al, 0x0D
    out    dx, al

    pop    rsi
    pop    rdx
    retn   8

;------------------------------------------------------
;arg1 - digit64
SerialPrintDigit64:
    push    rax
    push    rcx
    push    rdx

    mov    rdx, [rsp + 4 * 8]   ;arg 1
    xor    rcx, rcx
.next_part:
    cmp    cl, 16
    jz    .ext
    rol    rdx, 4
    mov    al, dl
    and    al, 0xF
    cmp    al, 0xA
    jae    .dig_A_F

.dig_0_9:
    add    al, 0x30
    jmp    .print

.dig_A_F:
    add    al, 0x57

.print:
    call   is_transmit_empty64

    push    rdx
    mov    dx, COM1
    out    dx, al
    pop    rdx

    inc    rcx
    jmp    .next_part
.ext:
    call    is_transmit_empty64
    mov    al, 0x0D
    mov    dx, COM1
    out    dx, al

    pop    rdx
    pop    rcx
    pop    rax
    retn    8