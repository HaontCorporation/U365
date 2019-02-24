#!/bin/bash
if test "$#" -ne 2; then
    echo -e " \e[1;34m==>\e[1;37m \e[31mSyntax error!\e[0m\n     Usage: $0 <binary name> <initrd desination path>"
    exit;
fi
echo -e " \e[1;34m==>\e[1;37m Compiling program for U365: $1. Destination path in initrd: $2"
cp ../build/bin/u365.elf .
rm -rf build
mkdir  build
gcc -o ../initrd/$2/$1 *.c -m32 -O3 -ffreestanding -Wall -Wextra -Wmaybe-uninitialized -fno-exceptions -nostdlib -nostartfiles -std=gnu11 -Wtype-limits -Llib -Iinclude -lc
./iso.sh u365.elf
qemu-system-i386 -cdrom u365.iso -m 256