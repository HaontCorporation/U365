
AS=nasm
CC=gcc
LD=ld

C_SOURCES=$(shell find -name "*.c")
ASM_SOURCES=$(shell find -name "*.asm")

OBJECTS=$(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

C_FLAGS=-Wall -masm=intel -ffreestanding -fno-omit-frame-pointer -fno-pic -mcmodel=kernel -mno-red-zone -c -g
AS_FLAGS=-felf64 -Fdwarf -g
LINKER=-nostdlib -no-pie -g -Tlinker.ld
INCLUDE=-Isrc/include -Ilib/include

all: link

%.o: %.c
	$(CC) $(INCLUDE) $(C_FLAGS) $< -o $@

%.o: %.asm
	$(AS) $(AS_FLAGS) $< -o $@

link: $(OBJECTS)
	$(LD) $(LINKER) $^ -o kernel.elf