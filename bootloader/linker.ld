ENTRY(main)

SECTIONS
{
	. = 0x7E00;

	.text : { *(.text) }
    .bss : { *(.bss) }
}
