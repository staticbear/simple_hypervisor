#pragma once

#define VPID                1

#define HYPERVISOR_RAM_MAX  0x140000000

#define HYPERVISOR_base     0x100000000
#define HYPERVISOR_size     0x100000

#define TABLE_SZ            0x1000
#define PAGE_SZ             0x1000

// entry counts
#define PDE_CNT             (HYPERVISOR_RAM_MAX / 0x200000)
#define PDPTE_CNT           (PDE_CNT / 512)
#define PML4E_CNT           1

// tables counts
#define PD_CNT              (PDE_CNT / 512)
#define PDPT_CNT            1
#define PML4_CNT            1

#define EPT_RAM_MAX         0x100000000

// entry counts
#define EPT_PTE_CNT         (EPT_RAM_MAX / 4096) 
#define EPT_PDE_CNT         (EPT_PTE_CNT / 512) 
#define EPT_PDPTE_CNT       4
#define EPT_PML4E_CNT       1

// tables counts
#define EPT_PT_CNT          (EPT_PTE_CNT / 512)  
#define EPT_PD_CNT          (EPT_PT_CNT  / 512)  
#define EPT_PDPT_CNT        1
#define EPT_PML4_CNT        1

#define EPT_STRUCT_SIZEx8   ((EPT_PML4_CNT * 512) + (EPT_PDPT_CNT * 512) + (EPT_PD_CNT * 512) + (EPT_PT_CNT * 512))

#define CACHE_TP_UC         0
#define CACHE_TP_WC         1
#define CACHE_TP_WT         4
#define CACHE_TP_WP         5
#define CACHE_TP_WB         6

#define DEV_RAM_addr        0xBA647000
#define DEV_RAM_size        (0x100000000 - 0xBA647000)

#define VIDEO_RAM0_addr     0xA0000
#define VIDEO_RAM0_size     (0xC0000 - 0xA0000)

#define VIDEO_RAM1_addr     0xD0000000
#define VIDEO_RAM1_size     (0xD2000000 - 0xD0000000)

#define VIDEO_RAM2_addr     0xC0000000
#define VIDEO_RAM2_size     (0xD0000000 - 0xC0000000)

#define VIDEO_RAM3_addr     0xFA000000
#define VIDEO_RAM3_size     (0xFB000000 - 0xFA000000)

#define TIMER_val           0x0FFFFFFF

#define CONFIG_ADDRESS      0xCF8
#define CONFIG_DATA         0xCFC