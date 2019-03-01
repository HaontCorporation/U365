
all: build_bootloader build_kernel run

build_bootloader:
	make -C bootloader

build_kernel:
	dd if=/dev/zero of=u365.img bs=512 count=2880
	mkdosfs -F 16 u365.img
	if ! grep -qs "/media/floppy" /proc/mounts; then \
		sudo mount -o loop u365.img /media/floppy; \
	fi  
	sudo cp bootloader/bin/stage2.bin /media/floppy/stage2.bin
	sudo umount /media/floppy/
	dd if=bootloader/bin/stage1.bin of=u365.img conv=notrunc

run:
	qemu-system-x86_64 -monitor stdio u365.img