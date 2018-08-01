OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386:x86-64)
ENTRY(main);
SECTIONS
{
	. = 0x100000000;
	.text : {*(.text);*(.rodata);}
}
