#!/bin/sh
if [ ! -f $1/$2gcc ]; then
        echo -e "\e[0;91mU365 build cannot run: no cross-GCC found!\e[0m\n\tPlease install it by instruction on \e[4mhttp://wiki.osdev.org/GCC_Cross-Compiler\e[0m."
	exit 1
fi
if [ ! -f $1/$2ld ]; then
        echo -e "\e[0;91mU365 build cannot run: no cross-Binutils found!\e[0m\n\tPlease install it by instruction on \e[4mhttp://wiki.osdev.org/GCC_Cross-Compiler\e[0m."
	exit 1
fi
exit 0

