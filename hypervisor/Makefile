all:
	gcc -O -c -m64 -masm=intel hypervisor.c long_mode.c serialport_log64.c inline_asm.c isr_wrapper.S interrupts.c vmx.c vmexit.c vmexit_wrapper.S
	ld -o hypervisor.hex -T main.ld hypervisor.o long_mode.o serialport_log64.o inline_asm.o isr_wrapper.o interrupts.o vmx.o vmexit.o vmexit_wrapper.o
	rm hypervisor.o long_mode.o serialport_log64.o inline_asm.o isr_wrapper.o interrupts.o vmx.o vmexit.o vmexit_wrapper.o
	