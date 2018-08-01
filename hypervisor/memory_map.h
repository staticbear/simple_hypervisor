#pragma once
#include "defines.h"

#define STACK64_size        0x1000
#define STACK64_addr        (HYPERVISOR_base + HYPERVISOR_size + STACK64_size)      

#define STACK64i_size       0x1000
#define STACK64i_addr       (STACK64_addr + STACK64i_size)              

#define GDT64_size          0x1000
#define GDT64_addr          STACK64i_addr                                

#define IDT64_size          0x1000
#define IDT64_addr          (GDT64_addr + GDT64_size)                    

#define PML4_size           (PML4_CNT * 512 *8)
#define PML4_addr           (IDT64_addr + IDT64_size)

#define PDPT_size           (PDPT_CNT * 512 * 8)
#define PDPT_addr           (PML4_addr + PML4_size)

#define PD_size             (PD_CNT * 512 * 8)
#define PD_addr             (PDPT_addr + PDPT_size)

#define TSS_size            0x1000
#define TSS_addr            (PD_addr + PD_size)

#define VMXON_size          0x1000
#define VMXON_addr          (TSS_addr + TSS_size)

#define VMCS_size           0x1000
#define VMCS_addr           (VMXON_addr + VMXON_size)

#define MSR_BMP_size        0x1000
#define MSR_BMP_addr        (VMCS_addr + VMCS_size)

#define IO_BMPA_size        0x1000
#define IO_BMPA_addr        (MSR_BMP_addr + MSR_BMP_size)

#define IO_BMPB_size        0x1000
#define IO_BMPB_addr        (IO_BMPA_addr + IO_BMPA_size)

#define MSR_EX_LDR_size     0x1000
#define MSR_EX_LDR_addr     (IO_BMPB_addr + IO_BMPB_size)       /* VM-exit MSR-load address */

#define MSR_EX_STR_size     0x1000
#define MSR_EX_STR_addr     (MSR_EX_LDR_addr + MSR_EX_LDR_size) /* VM-exit MSR-store address */

#define MSR_EN_LDR_size     0x1000
#define MSR_EN_LDR_addr     MSR_EX_STR_addr                     /* VM-entry MSR-load address */

#define EPT_PML4_size       (EPT_PML4_CNT * 512 * 8)
#define EPT_PML4_addr       (MSR_EN_LDR_addr + MSR_EN_LDR_size)

#define EPT_PDPT_size       (EPT_PDPT_CNT * 512 * 8)
#define EPT_PDPT_addr       (EPT_PML4_addr + EPT_PML4_size)

#define EPT_PD_size         (EPT_PD_CNT * 512 * 8)
#define EPT_PD_addr         (EPT_PDPT_addr + EPT_PDPT_size)

#define EPT_PT_size         (EPT_PT_CNT * 512 * 8)
#define EPT_PT_addr         (EPT_PD_addr + EPT_PD_size)

#define VAR_size            0x1000
#define VAR_addr            (EPT_PT_addr + EPT_PT_size)

#define GUEST_REGS_size     8 * 8                                /* rax, rcx, rdx, rbx, rsp, rbp, rsi , rdi */
#define GUEST_REGS_addr     VAR_addr

