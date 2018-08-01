#pragma once
#define IA32_APIC_BASE             0x1B
#define IA32_MTRR_PHYSBASE0        0x200
#define IA32_MTRR_PHYSMASK0        0x201
#define IA32_MTRR_DEF_TYPE         0x2FF
#define IA32_PAT                   0x277
#define IA32_EFER                  0xC0000080
#define IA32_FEATURE_CONTROL       0x3A
#define IA32_VMX_BASIC             0x480
#define IA32_VMX_MISC              0x485
#define IA32_VMX_CR0_FIXED0        0x486
#define IA32_VMX_CR0_FIXED1        0x487
#define IA32_VMX_CR4_FIXED0        0x488
#define IA32_VMX_CR4_FIXED1        0x489

#define IA32_VMX_TRUE_PINBASED_CTLS     0x48D
#define IA32_VMX_TRUE_PROCBASED_CTLS    0x48E
#define IA32_VMX_PROCBASED_CTLS2        0x48B
#define IA32_VMX_TRUE_EXIT_CTLS         0x48F
#define IA32_VMX_TRUE_ENTRY_CTLS        0x490
