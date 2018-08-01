#include "types.h"
#include "defines.h"
#include "memory_map.h"
#include "long_mode.h"
#include "serialport_log64.h"
#include "isr_wrapper.h"
#include "inline_asm.h"
#include "vmx.h"
#include "vmexit.h"

void main()
{
	// setup stack pointer
	asm volatile("mov rsp, %0"::"i"(STACK64_addr));
	
	InitLongModeGdt();
	
	InitLongModeIdt(get_isr_addr());
	
	InitLongModeTSS();
	
	InitLongModePages();
	
	InitControlAndSegmenRegs();

	if(!CheckVMXConditions())
		while(1);
	
	InitVMX();
	
	InitGuestRegisterState();
	
	InitGuestNonRegisterState();
	
	InitHostStateArea();
	
	InitEPT();
	
	if(!InitExecutionControlFields())
		while(1);
	
	if(!InitVMExitControl())
		while(1);
	
	if(!InitVMEntryControl())
		while(1);
	
	// enable PIC
	outb(0xA1, 0);
	outb(0x21, 0);
	
	VMLaunch();
	
	VMEnter_error();	
}
