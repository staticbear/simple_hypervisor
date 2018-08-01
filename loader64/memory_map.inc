

LOADER_addr         equ     0x9000
LOADER_size         equ     0x5000

IDT64_size          equ     0x1000
IDT64_addr          equ     (LOADER_addr + LOADER_size)            

INIT32_PML4_size    equ     (1 * 512 *8)
INIT32_PML4_addr    equ     (IDT64_addr + IDT64_size)

INIT32_PDPT_size    equ     (1 * 512 * 8)
INIT32_PDPT_addr    equ     (INIT32_PML4_addr + INIT32_PML4_size)

VAR_addr            equ     (INIT32_PDPT_addr + INIT32_PDPT_size)
VAR_size            equ     0x1000
HBA_MEM_addr        equ     (VAR_addr)
DRIVE_PORT          equ     (HBA_MEM_addr + 4)
ENTRIES_addr        equ     (DRIVE_PORT + 4)

TMP_RDBUF_addr      equ     (VAR_addr + VAR_size)
TMP_RDBUF_size      equ     0x2000

STACK_size          equ     0x1000
STACK_addr          equ     (TMP_RDBUF_addr + TMP_RDBUF_size + STACK_size)
