#!/bin/bash
if test "$#" -ne 1; then
    echo -e " \e[1;34m==>\e[1;37m \e[31mSyntax error!\e[0m\n     Usage: $0 <U365 binary>"
    exit;
fi
echo -e " \e[1;34m==>\e[1;37m Creating ISO from U365 binary '$1'"
rm -rf ../build/iso/fs
mkdir  ../build/iso/fs ../build/iso/fs/boot ../build/iso/fs/boot/grub
echo "insmod gfxterm;  \
insmod vbe;  \
timeout=5;   \
menuentry \"U365\"  \
{  \
	echo 'Loading kernel...';            \
	multiboot /boot/u365.elf;            \
	echo 'Loading initrd...'            ;\
	module    /boot/initrd.tar  initrd;  \
	boot;  \
}  \
" > ../build/iso/fs/boot/grub/grub.cfg
cp $1 iso_fs/boot/u365.elf
cd initrd
tar -cf ../iso_fs/boot/initrd.tar *
cd ..
grub-mkrescue -d /usr/lib/grub/i386-pc/ -o ../build/releases/u365.iso ../build/iso/fs