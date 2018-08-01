;http://wiki.osdev.org/Serial_Ports

COM1   equ     0x3f8

Init_Com_Port32:
	push   eax
	push   edx

	mov    al, 0x00
	mov    dx, COM1 + 1
	out    dx, al	   ; Disable all interrupts

	mov    al, 0x80
	mov    dx, COM1 + 3
	out    dx, al	   ; Enable DLAB (set baud rate divisor)

	mov    al, 14
	mov    dx, COM1 + 0
	out    dx, al	   ; Set divisor to 14 (lo byte) (8000 baud equ 9600 baud in my PC, I don't know why)

	mov    al, 0x00
	mov    dx, COM1 + 1
	out    dx, al	   ; (hi byte)

	mov    al, 0x03
	mov    dx, COM1 + 3
	out    dx, al	   ; 8 bits, no parity, one stop bit

	mov    al, 0xC7
	mov    dx, COM1 + 2
	out    dx, al	   ; Enable FIFO, clear them, with 14-byte threshold

	mov    al, 0x0B
	mov    dx, COM1 + 4
	out    dx, al	   ; IRQs enabled, RTS/DSR set

	pop    edx
	pop    eax
	ret

;------------------------------------------------------
is_transmit_empty32:
	push   eax
	push   edx

	mov    dx, COM1 + 5
.wait:
	in     al, dx
	test   al, 0x20
	jz     .wait

	pop    edx
	pop    eax
	ret
;------------------------------------------------------

;arg1 - addr of str (0 - end)
SerialPrintStr32:
	push   eax
	push   edx
	push   esi
	mov    esi, [esp + 4 * 4]   ;arg 1
.next_byte:

	call   is_transmit_empty32
	lodsb

	mov    dx, COM1
	out    dx, al
	test   al, al
	jnz    .next_byte

	pop    esi
	pop    edx
	pop    eax
	retn   4

;------------------------------------------------------
;arg1 - digit64
SerialPrintDigit32:
	push	eax
	push	ecx
	push	edx

	mov	edx, [esp + 4 * 4]   ;arg 1
	xor	ecx, ecx
.next_part:
	cmp	cl, 8
	jz	.ext
	rol	edx, 4
	mov	al, dl
	and	al, 0xF
	cmp	al, 0xA
	jae	.dig_A_F

.dig_0_9:
	add	al, 0x30
	jmp	.print

.dig_A_F:
	add	al, 0x57

.print:
	call   is_transmit_empty32

	push   edx
	mov    dx, COM1
	out    dx, al
	pop    edx

	inc    ecx
	jmp    .next_part
.ext:
	call	is_transmit_empty32
	mov	al, 0x0D
	mov	dx, COM1
	out	dx, al

	pop	edx
	pop	ecx
	pop	eax
	retn	4