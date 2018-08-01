// https://wiki.osdev.org/Inline_Assembly/Examples

#include "inline_asm.h"
#include "types.h"


inline void cpuid(DWORD code, DWORD* a, DWORD* c, DWORD* d, DWORD* b)
{
    asm volatile ( "cpuid" : "=a"(*a), "=c"(*c),"=d"(*d), "=b"(*b) : "0"(code));
}

/*---------------------------------------------------------------------------------------------------*/

inline void xsetbv(DWORD a, DWORD c, DWORD d)
{
    asm volatile ( "xsetbv" : : "a"(a), "c"(c), "d"(d));
}

/*---------------------------------------------------------------------------------------------------*/

inline void lidt(void* base, WORD size)
{ 
    struct {
        WORD  length;
        void* base;
    } __attribute__((packed)) IDTR = { size, base };
 
    asm ( "lidt %0" : : "m"(IDTR) ); 
}

/*---------------------------------------------------------------------------------------------------*/

inline void lgdt(void* base, WORD size)
{ 
    struct {
        WORD  length;
        void* base;
    } __attribute__((packed)) GDTR = { size, base };
 
    asm ( "lgdt %0" : : "m"(GDTR) );  
}

/*---------------------------------------------------------------------------------------------------*/

inline QWORD rdmsr(DWORD msr)
{
	DWORD low, high;
	asm volatile (
		"rdmsr"
		: "=a"(low), "=d"(high)
		: "c"(msr)
	);
	return ((QWORD)high << 32) | low;
}

/*---------------------------------------------------------------------------------------------------*/

inline void wrmsr(DWORD msr, QWORD value)
{
	DWORD low = value & 0xFFFFFFFF;
	DWORD high = value >> 32;
	asm volatile (
		"wrmsr"
		:
		: "c"(msr), "a"(low), "d"(high)
	);
}

/*---------------------------------------------------------------------------------------------------*/

inline void outb(WORD port, BYTE val)
{
    asm volatile ( "out %1, %0" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

/*---------------------------------------------------------------------------------------------------*/

inline BYTE inb(WORD port)
{
    BYTE ret;
    asm volatile ( "in %0, %1"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

/*---------------------------------------------------------------------------------------------------*/

inline void wrCR0(QWORD val)
{
    asm volatile ( "mov 	CR0, %0"::"r"(val) );
}

/*---------------------------------------------------------------------------------------------------*/

inline QWORD rdCR0()
{
    QWORD ret;
    asm volatile ( "mov 	%0, CR0":"=r"(ret));
    return ret;
}
/*---------------------------------------------------------------------------------------------------*/

inline void wrCR4(QWORD val)
{
    asm volatile ( "mov 	CR4, %0"::"r"(val) );
}

/*---------------------------------------------------------------------------------------------------*/

inline QWORD rdCR4()
{
    QWORD ret;
    asm volatile ( "mov 	%0, CR4":"=r"(ret));
    return ret;
}

/*---------------------------------------------------------------------------------------------------*/

inline bool vmxon(void* addr)
{
	QWORD flags;
    asm volatile( "vmxon 	%1\r\n" 
				  "pushf\n\t"
				  "pop %0"
				  : "=g"(flags) 
				  : "m"(addr)); 
	
    return flags & ((1UL << 1) | 		//CF
					(1UL << 6));		//ZF
}

/*---------------------------------------------------------------------------------------------------*/

inline bool vmclear(void* addr)
{
	QWORD flags;
    asm volatile( "vmclear 	%1\r\n" 
		  "pushf\n\t"
		  "pop %0"
		  : "=g"(flags) 
		  : "m"(addr)); 
	
    return flags & ((1UL << 1) | 		//CF
					(1UL << 6));		//ZF
}

/*---------------------------------------------------------------------------------------------------*/

inline bool vmptrld(void* addr)
{
	QWORD flags;
    asm volatile( "vmptrld 	%1\r\n" 
				  "pushf\n\t"
				  "pop %0"
				  : "=g"(flags) 
				  : "m"(addr)); 
	
    return flags & ((1UL << 1) | 		//CF
					(1UL << 6));		//ZF
}

/*---------------------------------------------------------------------------------------------------*/

inline bool vmwrite(QWORD index, QWORD value)
{
	QWORD flags;
    asm volatile( "vmwrite 	%1, %2\r\n" 
				  "pushf\n\t"
				  "pop %0"
				  : "=g"(flags) 
				  : "c"(index), "a"(value)); 
	
    return flags & ((1UL << 1) | 		//CF
					(1UL << 6));		//ZF
}

/*---------------------------------------------------------------------------------------------------*/

inline QWORD vmread(QWORD index)
{
	QWORD ret;
    asm volatile( "vmread 	%0, %1\r\n" 
				  : "=r"(ret)
				  : "c"(index)
				); 
	
    return ret;		
}

/*---------------------------------------------------------------------------------------------------*/

inline void VMLaunch()
{
	asm volatile( "mov     eax, 0x0000AA55\r\n"
				  "mov     ecx, 0x00090000\r\n"
				  "mov     edx, 0x80\r\n"
				  "mov     ebx, 0x00\r\n"
				  "mov     ebp, 0x00\r\n"
				  "mov     esi, 0x000E0000\r\n"
				  "mov     edi, 0x0000FFAC\r\n"
				  
				  "vmlaunch\r\n"
	);
}