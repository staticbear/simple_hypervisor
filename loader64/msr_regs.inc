IA32_APIC_BASE                  equ      0x1B
IA32_MTRR_PHYSBASE0             equ      0x200
IA32_MTRR_PHYSMASK0             equ      0x201
IA32_MTRR_DEF_TYPE              equ      0x2FF
IA32_PAT                        equ      0x277
IA32_EFER                       equ      0xC0000080
IA32_FEATURE_CONTROL            equ      0x3A
IA32_VMX_BASIC                  equ      0x480
IA32_VMX_MISC                   equ      0x485
IA32_VMX_CR0_FIXED0             equ      0x486
IA32_VMX_CR0_FIXED1             equ      0x487
IA32_VMX_CR4_FIXED0             equ      0x488
IA32_VMX_CR4_FIXED1             equ      0x489


IA32_VMX_TRUE_PINBASED_CTLS     equ      0x48D
IA32_VMX_TRUE_PROCBASED_CTLS    equ      0x48E
IA32_VMX_PROCBASED_CTLS2        equ      0x48B
IA32_VMX_TRUE_EXIT_CTLS         equ      0x48F
IA32_VMX_TRUE_ENTRY_CTLS        equ      0x490