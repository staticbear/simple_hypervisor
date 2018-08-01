
BUS_numb    equ    0x00
DEV_numb    equ    31
FUNC_numb   equ    2
PORT_numb   equ    0x03


include 'memory_map.inc'
include 'defines.inc'
include 'msr_regs.inc'

use16
org LOADER_addr
real_mode:
    cli
    xor    ax, ax
    mov    ds, ax
    mov    ss, ax
    mov    es, ax
    mov    sp, 0x7C00

    ; enable A20 gate
    in	  al, 92h
    test    al, 02h
    jnz    .no92
    or	  al, 02h
    out    92h, al

.no92:
    ; disable PIC
    mov    al, 0FFh
    out    0A1h, al
    out    21h, al

    ; imcr access
    mov    al, 0x70
    out    0x22, al
    mov    al, 0x01	       ; set bit 1 for SMP mode
    out    0x23, al

    lgdt    fword ptr GDTR

    mov    eax, CR0
    or	  al, 1
    mov    CR0, eax	       ; set bit 0 PE

    jmp    0008:main

rb real_mode+256-$

;----------------------------------------------------------

use32

GDT:
   NULL_descr	    db	  8 dup (0)

   CS_SEGMENT	    db	  0FFh, 0FFh   ; segment limit 15:0
		    db	  00h, 00h     ; segment base addr 15:0
		    db	  00h	       ; base 23:16
		    db	  10011011b    ;
		    db	  11001111b    ;
		    db	  00h	       ; base 31:24

   DATA_SEGMENT     db	  0FFh, 0FFh   ; segment limit 15:0
		    db	  00h, 00h     ; segment base addr 15:0
		    db	  00h	       ; base 23:16
		    db	  10010010b
		    db	  11001111b
		    db	  00h	       ; base 31:24

   CS64_SEGMENT     db	  00h, 00h     ; segment limit 15:0
		    db	  00h, 00h     ; segment base addr 15:0
		    db	  00h	       ; base 23:16
		    db	  98h
		    db	  20h
		    db	  00h	       ; base 31:24


   DS64_SEGMENT     db	  00h, 00h     ; segment limit 15:0
		    db	  00h, 00h     ; segment base addr 15:0
		    db	  00h	       ; base 23:16
		    db	  92h
		    db	  20h
		    db	  00h	       ; base 31:24

GDT_size	    equ $-GDT
GDTR		    dw	GDT_size-1
		    dd	GDT

;----------------------------------------------------------

include 'serialport_log32.inc'

msg_pm_active	    db	   'PM have been activated' ,0
msg_rd_ok	    db	   'read...ok' ,0
msg_err 	    db	   'read...err' ,0
msg_no_lm	    db	   'IA32e is not supported', 0
msg_lm_activated    db	   'Long Mode has been activated', 0
;----------------------------------------------------------

main:
    mov    ax, 10h
    mov    ds, ax
    mov    es, ax
    mov    ss, ax
    mov    fs, ax
    mov    gs, ax
    mov    esp, STACK_addr

    call   Init_Com_Port32

    push   msg_pm_active
    call   SerialPrintStr32

    mov    eax, 0x80000000	   ; Extended-function 8000000h.
    cpuid			   ; Is largest extended function
    cmp    eax, 0x80000000	   ; any function > 80000000h?
    ja	  .lm_supported
    mov    eax, 0x80000001	   ; Extended-function 8000001h.
    cpuid			   ; Now EDX = extended-features flags.
    bt	   edx, 29		    ; Test if long mode is supported.
    jc	  .lm_supported

    push    msg_no_lm
    call    SerialPrintStr32
    jmp    $

.lm_supported:
    ; make PML4
    mov    edi, INIT32_PML4_addr
    mov    eax, (INIT32_PDPT_addr or 7)
    stosd
    xor    eax, eax
    stosd

    ; make Page Directory Pointer Table
    mov    edi, INIT32_PDPT_addr
    mov    eax, 0x87		    ; 1 GB Page
    xor    edx, edx
    mov    ecx, PDPTE_CNT
.nxt_pdpte:
    stosd
    xchg   eax, edx
    stosd
    xchg   eax, edx
    add    eax, 0x40000000
    jnc    @f
    inc    edx
@@:
    dec    ecx
    jnz    .nxt_pdpte

    mov    eax, CR4
    or	   eax, 0xA0		       ; set PAE(5), PGE(7)
    mov    CR4, eax

    mov    eax, INIT32_PML4_addr
    mov    CR3, eax

    mov    ecx, IA32_EFER
    rdmsr
    or	   eax, 0x00000100	      ; set LME (8)
    wrmsr

    mov    eax, CR0
    or	   eax, 0xE0000000	      ; set PG(31), CD(30), NE(29)
    mov    CR0, eax

    jmp    3*8:long_mode

use64
long_mode:

    mov    ax, 4 * 8
    mov    ds, ax
    mov    es, ax
    mov    ss, ax
    mov    fs, ax
    mov    gs, ax

    push    msg_lm_activated
    call    SerialPrintStr64

    ; make IDT
    mov    rdi, IDT64_addr
    mov    rdx, common_int
    mov    rax, 0x8E0100080000	      ; present and type = 1110 interrupt gate
    mov    ax, dx		      ; offset 15..0
    and    edx, 0xFFFF0000
    shl    rdx, 32
    or	   rax, rdx		       ; offset 31..16
    mov    rdx, common_int
    shr    rdx, 32
    mov    rcx, 32
.nxt_idte:
    stosq
    xchg   rax, rdx
    stosq
    xchg   rax, rdx
    dec    rcx
    jnz    .nxt_idte

    xor    rax, rax
    push   rax
    mov    rax, IDT64_addr
    shl    rax, 16
    mov    ax, 0x1FF
    push   rax
    lidt   [rsp]
    add    rsp, 16

    push    0x24		      ; register  (BAR5)  AHCI base memory
    push    FUNC_numb		      ; function
    push    DEV_numb		      ; device
    push    BUS_numb		      ; bus
    call    pciConfigReadWord64

    mov    [HBA_MEM_addr], eax

    push    PORT_numb
    call    ahci_init64
    jnc    .err

    ;read entries from 1 sector to 0x8E00
    push    ENTRIES_addr	      ; mem base addr to read
    push    1			      ; n sectors to read
    push    1			      ; start sector
    call    ReadEntryData
    jnc    .err

    ;format entries. entries count - 32 max , FFFFFFFF XXXXXXXX XXXXXXXX XXXXXXXX - last entry
    ; ===========================================
    ;  [first sector]  [n sectors]    [ram addr]
    ;  [4 bytes]       [4 bytes]      [8 bytes]
    ; ===========================================
    mov    esi, ENTRIES_addr
    cmp    dword ptr esi, 0xFFFFFFFF
    jz	  .err
.next_entry:
    lodsd
    cmp    eax, 0xFFFFFFFF
    jz	  .jmp_to_first_entry
    mov    edx, eax
    lodsd
    mov    ecx, eax
    lodsq

    push   rax			 ; mem base addr to read
    push   rcx			 ; n sectors to read
    push   rdx			 ; start sector
    call   ReadEntryData
    jc	  .next_entry
.err:
    push   msg_err
    call   SerialPrintStr32
    jmp    $

.jmp_to_first_entry:
    push   msg_rd_ok
    call   SerialPrintStr64

    call   ahci_uninit64

    mov    rax, qword ptr ENTRIES_addr + 8
    jmp    rax


;----------------------------------------------------------

; input: stack arg1 - start sector
; stack arg2 - n sectors to read
; stack arg3 - mem base addr to read
; output: CF - 1(ok)/0(err)
ReadEntryData:
    push    rdx
    push    rcx
    push    rdi
    push    rsi
    push    rax

    mov    edx, dword ptr rsp + (1 + 5) * 8  ; start sector
    mov    ecx, dword ptr rsp + (2 + 5) * 8  ; n sectors to read
    mov    rdi, qword ptr rsp + (3 + 5) * 8  ; mem base addr to read
.next_part:
    test    ecx, ecx
    jz	  .ok

    push    rdx
    call    ReadDisk
    jnc    .ext

; copy data from temporary buffer into end address
    mov    eax, ecx
    mov    rsi, TMP_RDBUF_addr
    cmp    ecx, 16
    ja	  .cp_a
.cp_b:				     ; ecx < 16
    shl    ecx, 9		     ; ecx * 512
    xor    eax, eax
    jmp    .start_cp
.cp_a:				     ; ecx > 16
    mov    ecx, 16 * 512	     ; ecx = 16 * 512
    sub    eax, 16
.start_cp:
    rep    movsb
    mov    ecx, eax
    jmp    .next_part
.ok:
    stc
.ext:
    pop    rax
    pop    rsi
    pop    rdi
    pop    rcx
    pop    rdx
    retn    3 * 8

;----------------------------------------------------------

; input: stack arg1 - first sector
; output: CF - 1(ok)/0(err)
; read data into TMP_RDBUF_addr, size = 8K
ReadDisk:
    push   rax
    mov    eax, [rsp + 2 * 8]
    push   TMP_RDBUF_addr
    push   0
    push   rax
    call   ahci_read64
    pop    rax
    retn   1 * 8

;----------------------------------------------------------

common_int:
    mov    rax, 0xDEADC0DE
    push   rax
    call   SerialPrintDigit64
    jmp    $


include 'ahci64.inc'
include 'serialport_log64.inc'




