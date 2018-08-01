#pragma once
#include "types.h"

void cpuid(DWORD code, DWORD* a, DWORD* b, DWORD* c, DWORD* d);
void xsetbv(DWORD a, DWORD c, DWORD d);
void lidt(void* base, WORD size);
void lgdt(void* base, WORD size);
QWORD rdmsr(DWORD msr_id);
void wrmsr(DWORD msr_id, QWORD msr_value);
void outb(WORD port, BYTE val);
BYTE inb(WORD port);
void wrCR0(QWORD val);
QWORD rdCR0(void);
void wrCR4(QWORD val);
QWORD rdCR4(void);
bool vmxon(void* addr);
bool vmclear(void* addr);
bool vmptrld(void* addr);
bool vmwrite(QWORD index, QWORD value);
QWORD vmread(QWORD index);
void VMLaunch(void);