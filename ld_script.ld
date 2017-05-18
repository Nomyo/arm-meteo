ENTRY(_start)

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2M
	RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 256K
}

stack_size = 1024;
heap_size = 256;

_stack_start = ORIGIN(RAM) + LENGTH(RAM);
_stack_end = _stack_start - stack_size;

SECTIONS
{
	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >FLASH

	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)
		*(.eh_frame)
		KEEP (*(.init))
		KEEP (*(.fini))
		. = ALIGN(4);
	} >FLASH

	_sidata = .;

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data*)
		*(.romtext)
		. = ALIGN(4);
		_edata = .;
	} >RAM AT>FLASH

	. = ALIGN(4);

	.bss :
	{
		_sbss = .;
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = _ebss;
	} >RAM

	. = ALIGN(4);

	.heap :
	{
		_heap_start = .;
		. = . + heap_size;
		_heap_end = .;
	} >RAM
}
