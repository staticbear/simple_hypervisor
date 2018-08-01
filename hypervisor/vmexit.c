#include "vmexit.h"
#include "defines.h"
#include "types.h"
#include "defines.h"
#include "memory_map.h"
#include "msr_regs.h"
#include "inline_asm.h"
#include "serialport_log64.h"
#include "vmx.h"

static void CPUID_hndlr()
{
	QWORD *ptrGUEST_REGS = (QWORD *)GUEST_REGS_addr;
	
	DWORD eax_v, ecx_v, edx_v, ebx_v;
	DWORD cpuid_init = (DWORD)ptrGUEST_REGS[0];
	
	if(cpuid_init == 0x80000002){
		eax_v = 0x20584D56;
		ebx_v = 0x64757453; 
		ecx_v = 0x6F432079;
		edx_v = 0x293A6572;
	}
	else if(cpuid_init == 0x80000003 || cpuid_init == 0x80000004){
		eax_v = 0;
		ebx_v = 0; 
		ecx_v = 0;
		edx_v = 0;
	}
	else
		cpuid(cpuid_init, &eax_v, &ecx_v, &edx_v, &ebx_v);
	
	ptrGUEST_REGS[0] = eax_v;
	ptrGUEST_REGS[1] = ecx_v;
	ptrGUEST_REGS[2] = edx_v;
	ptrGUEST_REGS[3] = ebx_v;

	QWORD vm_rd_val;
	/* add cpuid length to Guest RIP 0000681EH */
	vm_rd_val = vmread(0x681E);
	vm_rd_val += 2;
	vmwrite(0x681E, vm_rd_val);
	
	return;
}

/*---------------------------------------------------------------------------------------------------*/

static bool CR_Reg_hndlr()
{
	QWORD vm_rd_val = vmread(0x6400);							/* Exit qualification */
	
	if((vm_rd_val & 0xFF) != 0){															
		SerialPrintStr64("Access to CR: unexpected case");
		return FALSE;
	}
	
	QWORD *ptrGUEST_REGS = (QWORD *)GUEST_REGS_addr;
	BYTE gp_reg_n = (BYTE)((vm_rd_val >> 8) & 0xF);				/* [11:8] - Number of general-purpose register */
	
	QWORD gp_reg_val;
	if(gp_reg_n == 4)											/* RSP case, it's strange, but still possible */
		gp_reg_val = vmread(0x681C);							/* Guest RSP */
	else
		gp_reg_val = ptrGUEST_REGS[gp_reg_n];

	/* CR0 read shadow 00006004H */
	vmwrite(0x6004, gp_reg_val);

	/* Guest CR0 00006800H */
	vmwrite(0x6800, gp_reg_val & 0x9FFFFFFF);					/* clear CD and WT */
	
	/* add mov CRn length to Guest RIP 0000681EH */
	vm_rd_val = vmread(0x681E);									
	vm_rd_val += 3;
	vmwrite(0x681E, vm_rd_val);

	return TRUE;
}

/*---------------------------------------------------------------------------------------------------*/

static void XSETBV_hndlr()
{
	QWORD *ptrGUEST_REGS = (QWORD *)GUEST_REGS_addr;
	
	xsetbv((DWORD)ptrGUEST_REGS[0], (DWORD)ptrGUEST_REGS[1], (DWORD)ptrGUEST_REGS[2]);
	
	/* add xsetbv length to Guest RIP 0000681EH */
	QWORD vm_rd_val = vmread(0x681E);									
	vm_rd_val += 3;
	vmwrite(0x681E, vm_rd_val);
	
	return;
}

/*---------------------------------------------------------------------------------------------------*/

void VMEXIT_handler()
{
	QWORD vm_rd_val;
	
	/* determine exit reason 00004402H */
	vm_rd_val = vmread(0x4402);
	
	switch(vm_rd_val & 0xFFFF)
	{
		
		/* CPUID. Guest software attempted to execute CPUID. */
		case 10:{
			CPUID_hndlr();
			break;
		}
		/* Control-register accesses. */
		case 28:{
			if(!CR_Reg_hndlr()){
				while(1);
			}
			else
				break;
		}
		/* XSETBV. Guest software attempted to execute XSETBV */
		case 55:{
			XSETBV_hndlr();
			break;
		}
		default:{
			SerialPrintStr64("VMEXIT: unexpected case");
			SerialPrintDigit64(vm_rd_val);
			while(1);
		}
	}
	
	return;
}

/*---------------------------------------------------------------------------------------------------*/

void VMEnter_error()
{
	QWORD vm_rd_val = vmread(0x6400);							/* Exit qualification */
	SerialPrintDigit64(vm_rd_val);
	
	vm_rd_val = vmread(0x4402);									/* Exit reason */
	SerialPrintDigit64(vm_rd_val);
	
	while(1);
}