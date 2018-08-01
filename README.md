Simple hypervisor with using Intel VMX, written on pure C with ASM inlines. Compiled with GCC 7.3.0<br/>

Features:<br/>
	Tiny size<br/>
	Any external libs non used<br/>
	Guest OS start running in virtual mode begin from first commands.<br/>
	
Accepted guest OS: Windows 7 x32 with next limits:<br/>
	single logical core used<br/>
	PAE options disabled<br/>
	UEFI disabled<br/>
