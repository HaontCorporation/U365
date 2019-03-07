
all: compile

compile:
	nasm -fbin src/stage1.asm -o bin/stage1.bin
	nasm -felf32 src/stage2.asm -o bin/stage2-asm.o
	i686-elf-gcc -Wall -ffreestanding -nostdlib -c src/stage2.c -o bin/stage2-c.o
	i686-elf-gcc -nostdlib -nostartfiles -g -Tlinker.ld bin/stage2-asm.o bin/stage2-c.o -o bin/stage2.elf -lgcc
	
	objcopy -S -O binary bin/stage2.elf bin/stage2.bin