
CONFIG_ADDRESS		equ	0xCF8
CONFIG_DATA			equ	0xCFC

SIZE_OF_PORT		equ	0x80


SATA_SIG_ATA		equ	0x00000101	; SATA drive
SATA_SIG_ATAPI		equ	0xEB140101	; SATAPI drive
SATA_SIG_SEMB		equ	0xC33C0101	; Enclosure management bridge
SATA_SIG_PM			equ	0x96690101	; Port multiplier
 
AHCI_DEV_NULL		equ	0
AHCI_DEV_SATA		equ	1
AHCI_DEV_SEMB		equ	2
AHCI_DEV_PM			equ	3
AHCI_DEV_SATAPI 	equ	4
 
HBA_PORT_IPM_ACTIVE		equ	1
HBA_PORT_DET_PRESENT	equ	3

ATA_CMD_READ_DMA_EX		equ	 0x25
ATA_CMD_WRITE_DMA_EX	equ	 0x35

HBA_PxIS_TFES		equ	 0x40000000
HBA_PxCMD_CR		equ	 0x8000
HBA_PxCMD_ST		equ	 0x0001
HBA_PxCMD_FRE		equ	 0x0010
HBA_PxCMD_FR		equ	 0x4000

ATA_DEV_BUSY		equ	 0x80
ATA_DEV_DRQ			equ	 0x08

FIS_TYPE_REG_H2D	equ	 0x27	; Register FIS - host to device
FIS_TYPE_REG_D2H	equ	 0x34	; Register FIS - device to host
FIS_TYPE_DMA_ACT	equ	 0x39	; DMA activate FIS - device to host
FIS_TYPE_DMA_SETUP	equ	 0x41	; DMA setup FIS - bidirectional
FIS_TYPE_DATA		equ	 0x46	; Data FIS - bidirectional
FIS_TYPE_BIST		equ	 0x58	; BIST activate FIS - bidirectional
FIS_TYPE_PIO_SETUP	equ	 0x5F	; PIO setup FIS - device to host
FIS_TYPE_DEV_BITS	equ	 0xA1	; Set device bits FIS - device to host

;------------------------------------------------------

; Port Registers (one set per port) HBA_PORT
PxCLB		equ	00h   ; Port x Command List Base Address
PxCLBU		equ	04h   ; Port x Command List Base Address Upper 32-Bits
PxFB		equ	08h   ; Port x FIS Base Address
PxFBU		equ	0Ch   ; Port x FIS Base Address Upper 32-Bits
PxIS		equ	10h   ; Port x Interrupt Status
PxIE		equ	14h   ; Port x Interrupt Enable
PxCMD		equ	18h   ; Port x Command and Status
Reserved	equ	1Ch   ; Reserved
PxTFD		equ	20h   ; Port x Task File Data
PxSIG		equ	24h   ; Port x Signature
PxSSTS		equ	28h   ; Port x Serial ATA Status (SCR0: SStatus)
PxSCTL		equ	2Ch   ; Port x Serial ATA Control (SCR2: SControl)
PxSERR		equ	30h   ; Port x Serial ATA Error (SCR1: SError)
PxSACT		equ	34h   ; Port x Serial ATA Active (SCR3: SActive)
PxCI		equ	38h   ; Port x Command Issue
PxSNTF		equ	3Ch   ; Port x Serial ATA Notification (SCR4: SNotification)
PxFBS		equ	40h   ; Port x FIS-based Switching Control
PxDEVSLP	equ	44h   ; Port x Device Sleep
Reserved	equ	48h   ; Reserved
PxVS		equ	70h   ; Port x Vendor Specific

;------------------------------------------------------

;typedef struct tagFIS_REG_H2D

fis_type	equ	00h	; FIS_TYPE_REG_H2D
c			equ	01h	; 1: Command, 0: Control
command 	equ	02h	; Command register
featurel	equ	03h	; Feature register, 7:0

lba0		equ	04h	; LBA low register, 7:0
lba1		equ	05h	; LBA mid register, 15:8
lba2		equ	06h	; LBA high register, 23:16
device		equ	07h	; Device register

lba3		equ	08h	; LBA register, 31:24
lba4		equ	09h	; LBA register, 39:32
lba5		equ	0Ah	; LBA register, 47:40
featureh	equ	0Bh	; Feature register, 15:8

countl		equ	0Ch	; Count register, 7:0
counth		equ	0Dh	; Count register, 15:8
icc		equ	0Eh	; Isochronous command completion
control 	equ	0Fh	; Control register

;------------------------------------------------------

; HBA_CMD_HEADER
CMD_HEADER_DW0	equ	 00h
CMD_HEADER_DW1	equ	 04h
CMD_HEADER_DW2	equ	 08h
CMD_HEADER_DW3	equ	 0Ch

;------------------------------------------------------

sizeofPRDT	equ	0x10
sizeofCMDTBL	equ	0x80

;------------------------------------------------------

; input:
; arg1 - bus
; arg2 - device
; arg3 - function
; arg4 - register
; output EAX - data from register
pciConfigReadWord64:
	push	rdx

	mov	eax, 0x80000000
	mov	dl, [rsp + 2 * 8]   ; bus
	shl	edx, 16
	or	eax, edx
	mov	dl, [rsp + 3 * 8]   ; device
	movzx	edx, dl
	shl	edx, 11
	or	eax, edx

	mov	dl, [rsp + 4 * 8]   ; function
	shl	edx, 8
	movzx	edx, dx
	mov	dl, [rsp + 5 * 8]   ; register
	and	dl, 0xFC	    ; align by 4
	or	eax, edx

	mov	dx, CONFIG_ADDRESS
	out	dx, eax

	mov	dx, CONFIG_DATA
	in	eax, dx

	pop	rdx
	retn	4 * 8

;------------------------------------------------------

; input:
; arg1 - address of port
; output:
; eax - uint32 type
check_type64:
	push	rbx
	mov	ebx, [rsp + 2 * 8]

	mov	eax, [ebx + 0x28]	 	; ssts
	and	al, 0x0F		 		; Device Detection (DET)
	cmp	al, HBA_PORT_DET_PRESENT
	jnz	.dev_np

	mov	eax, [ebx + 0x28]	 	; ssts
	shr	eax, 8
	and	al, 0x0F		 		; Interface Power Management (IPM)
	cmp	al, HBA_PORT_IPM_ACTIVE
	jnz	.dev_np

	mov	eax, [ebx + 0x24]	 	; signature

	push	AHCI_DEV_SATAPI
	cmp	eax, SATA_SIG_ATAPI
	pop	rax
	jz	.ext

	push	AHCI_DEV_SEMB
	cmp	eax, SATA_SIG_SEMB
	pop	rax
	jz	.ext

	push	AHCI_DEV_PM
	cmp	eax, SATA_SIG_PM
	pop	rax
	jz	.ext
	mov	eax, AHCI_DEV_SATA
	jmp	.ext
.dev_np:
	mov	eax, AHCI_DEV_NULL
.ext:
	pop	rbx
	retn	1 * 8

;------------------------------------------------------

; input:
; arg1 - port number with driver
; output CF = 1 ok / CF = 0 - err
ahci_init64:
	push	rax
	push	rcx
	push	rbx

	mov	ecx, [rsp + (1 + 3) * 8] 	; arg1 = port number
	mov	ebx, [HBA_MEM_addr]
	mov	eax, [ebx + 3 * 4]	 		; eax = pi = abar->pi;
	bt	eax, ecx
	jnc	.ext			 			; if (pi & bit_position)  == 0
	shl	ecx, 7			 			; ecx = port number * SIZE_OF_PORT (0x80)
	lea	ebx, [ebx + ecx + 0x100] 	; HBA_PORT offset
	mov	[DRIVE_PORT], ebx
	push	rbx
	call	check_type64
	cmp	al, AHCI_DEV_SATA
	clc
	jnz	 .ext
	; Wait until CR (bit15) is cleared
.wt:
	test	dword ptr ds:ebx + PxCMD, HBA_PxCMD_CR	; while (port->cmd & HBA_PxCMD_CR);
	jnz	.wt
	or	dword ptr ds:ebx + PxCMD, HBA_PxCMD_FRE  	; Set FRE (bit4)
	or	dword ptr ds:ebx + PxCMD, HBA_PxCMD_ST	 	; Set FRE (bit4) and ST (bit0)
	stc
.ext:
	pop	rbx
	pop	rcx
	pop	rax
	retn	1 * 8

;------------------------------------------------------

ahci_uninit64:

	push	rbx

	mov	ebx, [DRIVE_PORT]
	and	dword ptr ds:ebx + PxCMD, not HBA_PxCMD_ST
	and	dword ptr ds:ebx + PxCMD, not HBA_PxCMD_FRE
.wt1:
	test	dword ptr ds:ebx + PxCMD, HBA_PxCMD_FR
	jnz	.wt1

	test	dword ptr ds:ebx + PxCMD, HBA_PxCMD_CR
	jnz	.wt1



	pop	rbx
	ret

;------------------------------------------------------

; input:
; arg1 - port addr
; output:
; eax - free slot / -1 - error
find_cmdslot64:
	push	rcx
	push	rbx
	mov	ebx, [rsp + 3 * 8]		; arg1 = port addr
	mov	ecx, [ebx + PxSACT]
	or	ecx, [ebx + PxCI]		; slots = (m_port->sact | m_port->ci);
	xor	eax, eax
.check_nxt_slot:
	test	cl, 1
	jz	.ext

	shr	ecx, 1
	inc	eax

	cmp	al, 32
	jnz	.check_nxt_slot

	xor	eax, eax
	dec	ecx
.ext:
	mov	eax, ecx
	pop	rbx
	pop	rcx
	retn	1 * 8

;------------------------------------------------------

; input:
; arg1 - startl
; arg2 - starth
; arg3 - rdBuf addr
; output CF = 1 ok / CF = 0 - err
ahci_read64:
	push	rax
	push	rbx
	push	rcx
	push	rdi
	push	rbp

	; clear pending interrupt bits
	mov	ebx, [DRIVE_PORT]
	mov	dword ptr ds:ebx + PxIS, 0xFFFFFFFF

	push	rbx
	call	find_cmdslot64
	cmp	eax, 0xFFFFFFFF
	jz	.err
	mov	ebp, eax					; save slot number into ebp

	mov	ebx, [ebx + PxCLB]			; ebx = command list base addr
	shl	eax, 5						; eax = slot number * slot_size(20h)
	add	ebx, eax					; ebx = command header

	mov	eax, 0x10005				; PRDTL 31..16	= 1 | sizeof(FIS_REG_H2D)/sizeof(uint32_t) = 5
	mov	[ebx + 0], eax

	mov	ebx, [ebx + 8]				; ebx = ctba  (command table base addr)
	mov	ecx, 0x24					; (sizeofCMDTBL + sizeofPRDT) / 4
	mov	edi, ebx
	xor	eax, eax
	rep	stosd						; memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl)*sizeof(HBA_PRDT_ENTRY));

	mov	eax, [rsp + (3 + 5) * 8]	; arg3 = rdbudf
	push	rbx
	lea	ebx, [ebx + 0x80]			; first PRDT offset
	mov	[ebx], eax					; dba = rdbuf
	mov	dword ptr ds:ebx + 0xC, (8 * 1024 - 1) or 0x80000000 ; dbc.i = 1, dbc = 8kbytes

	;setup command
	pop	rbx													; ebx = ctba  (command table base addr)
	mov	byte ptr ds:ebx + fis_type, FIS_TYPE_REG_H2D	  	; cmdfis->fis_type = FIS_TYPE_REG_H2D;
	mov	byte ptr ds:ebx + c, 0x80 							; cmdfis->c = 1;  // Command
	mov	byte ptr ds:ebx + command, ATA_CMD_READ_DMA_EX

	mov	eax, [rsp + (1 + 5) * 8]							; arg1 = startl
	mov	byte ptr ds:ebx + lba0, al							; (uint8_t)startl;
	shr	eax, 8
	mov	byte ptr ds:ebx + lba1, al							; (uint8_t)(startl>>8);
	shr	eax, 8
	mov	byte ptr ds:ebx + lba2, al							; (uint8_t)(startl>>16);
	shr	eax, 8
	mov	byte ptr ds:ebx + device, 0x40						; 1<<6;  // LBA mode

	mov	byte ptr ds:ebx + lba3, al							; (uint8_t)(startl>>24);
	mov	eax, [rsp + (2 + 5) * 8]							; arg2 = starth
	mov	byte ptr ds:ebx + lba4, al							; (uint8_t)starth;
	shr	eax, 8
	mov	byte ptr ds:ebx + lba5, al							; (uint8_t)(starth>>8);

	mov	eax, 16 											; count =  6 sectors to read
	mov	byte ptr ds:ebx + countl, al						; count & 0xFF;
	shr	eax, 8
	mov	byte ptr ds:ebx + counth, al						; (count >> 8) & 0xFF;

	; The below loop waits until the port is no longer busy before issuing a new command
	mov	ebx, [DRIVE_PORT]									; arg1 ebx = port
.wt2:
	mov	eax, dword ptr ds:ebx + PxTFD
	and	eax, ATA_DEV_BUSY or ATA_DEV_DRQ
	jnz	.wt2

	xor	eax, eax
	bts	eax, ebp											; 1<<slot;
	mov	dword ptr ds:ebx + PxCI, eax						; port->ci = 1<<slot;	// Issue command
.wt3:
	test	dword ptr ds:ebx + PxIS, HBA_PxIS_TFES
	jnz	.err
	test	dword ptr ds:ebx + PxCI, eax
	jnz	.wt3
	stc
	jmp	.ext
.err:
	clc
.ext:
	pop	rbp
	pop	rdi
	pop	rcx
	pop	rbx
	pop	rax
	retn	3 * 8
