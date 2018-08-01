Simple hypervisor with using Intel VMX, written on pure C with ASM inlines. Compiled with GCC 7.3.0<br/>

Features:<br/>
- Tiny size
- Any external libs non used
- Guest OS start running in virtual mode begin from first commands.
	
Accepted guest OS: Windows 7 x32 with next limits:
- single logical core used
- PAE options disabled
- UEFI disabled
