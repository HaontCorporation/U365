
all: compile_os run_without_gdb

build_bootloader:
	make -C bootloader

build_kernel:
	make -C kernel

compile_os: build_bootloader build_kernel
	objcopy -O binary kernel/kernel.elf kernel/kernel.bin

	dd if=/dev/zero of=u365.img bs=512 count=2880
	mkdosfs -F 16 u365.img
	if ! grep -qs "/media/floppy" /proc/mounts; then \
		sudo mount -o loop u365.img /media/floppy; \
	fi  
	sudo cp bootloader/bin/stage2.bin /media/floppy/stage2.bin
	sudo cp kernel/kernel.elf /media/floppy/kernel.elf
	sudo umount -l /media/floppy/
	dd if=bootloader/bin/stage1.bin of=u365.img conv=notrunc
	rm -f *.o

run_with_gdb:
	gnome-terminal -e "bash -c \"qemu-system-x86_64 -S -s -boot u365.img -d cpu_reset\""

run_without_gdb:
	qemu-system-x86_64 u365.img -d cpu_reset
