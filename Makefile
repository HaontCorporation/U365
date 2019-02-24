ARROW=\e[1;34m==>\e[1;37m

#U365 Makefile configuration

LSCRIPT    = link
BUILD_DIR  = build
SRC_DIR    = src
INC_DIR    = include
OS_VER     = 1.2

#Filenames

ARCH       = i686
BINFORMAT  = elf
BITS       = 32
ISODIR     = $(BUILD_DIR)/iso/fs
GRUB_BIN   = /usr/lib/grub/i386-pc/
CROSS      = ~/opt/cross
CROSSBIN   = $(CROSS)/bin/
GAS        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-as #AT&T-syntax assembler
CC         = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-gcc
CXX        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-g++
IASM       = nasm #Intel-syntax assembler
ISOFILE    = u365-$(OS_VER)#Compiled ELF binary and ISO image name.
BINFILE    = u365
EMU        = qemu-system-i386
EFLAGS     = -cdrom $(BUILD_DIR)/releases/$(ISOFILE).iso -m 256 -drive file=ata_test.hdd,format=raw -usbdevice tablet -device rtl8139,id=u365_nic0
DEFS       = -DOSVER="$(OS_VER)"
MKRSCFLAGS = -d $(GRUB_BIN) -o $(BUILD_DIR)/releases/$(ISOFILE).iso $(ISODIR)
CFLAGS     = -O3 -ffreestanding -Wall -Wextra -Werror -Wmaybe-uninitialized -fno-exceptions -Iinclude -Iinclude/libc -Iinitrd/usr/include -std=gnu11    -c $(DEFS) -Wtype-limits -g
CXXFLAGS   = -O3 -ffreestanding -Wall -Wextra -Werror -Wmaybe-uninitialized -fno-exceptions -Iinclude -Iinclude/stl -Iinclude/libc -c $(DEFS) -Wtype-limits -g
GASFLAGS   =
IASFLAGS   = -f $(BINFORMAT)
LDFLAGS    = -T $(LSCRIPT).ld -o $(BUILD_DIR)/bin/$(BINFILE).$(BINFORMAT) -O3 -nostdlib -Wl,-Map=System.map -lgcc

# Source file names computing
# Multiboot header first
SOURCES  := $(SRC_DIR)/arch/$(ARCH)/boot.asm
# Al common sources
SOURCES  += $(shell find $(SRC_DIR) -name "*.c"   -and -not -path "$(SRC_DIR)/arch/*" -type f -print)
SOURCES  += $(shell find $(SRC_DIR) -name "*.cpp" -and -not -path "$(SRC_DIR)/arch/*"  -type f -print)
SOURCES  += $(shell find $(SRC_DIR) -name "*.s"   -and -not -path "$(SRC_DIR)/arch/*" -type f -print)
SOURCES  += $(shell find $(SRC_DIR) -name "*.asm" -and -not -path "$(SRC_DIR)/arch/*" -type f -print)
# Architecture-dependent sources
SOURCES  += $(shell find $(SRC_DIR)/arch/$(ARCH) -name '*.c'   -type f -print)
SOURCES  += $(shell find $(SRC_DIR)/arch/$(ARCH) -name '*.cpp' -type f -print)
SOURCES  += $(shell find $(SRC_DIR)/arch/$(ARCH) -name '*.s'   -type f -print)
SOURCES  += $(shell find $(SRC_DIR)/arch/$(ARCH) -name '*.asm' -and -not -path "$(SRC_DIR)/arch/$(ARCH)/boot.asm" -type f -print)

SRCDIRS  := $(shell find $(SRC_DIR) -type d -print)
OBJDIRS  := $(patsubst $(SRC_DIR)/%,"$(BUILD_DIR)/obj/%",$(SRCDIRS))

FAKE := $(shell echo $(SOURCES))
# Object file names computing
OBJS     := $(patsubst $(SRC_DIR)/%.c,"$(BUILD_DIR)/obj/%.c.o",$(SOURCES))
OBJS     := $(patsubst $(SRC_DIR)/%.cpp,"$(BUILD_DIR)/obj/%.cpp.o",$(OBJS))
OBJS     := $(patsubst $(SRC_DIR)/%.s,"$(BUILD_DIR)/obj/%.s.o",$(OBJS))
OBJS     := $(patsubst $(SRC_DIR)/%.asm,"$(BUILD_DIR)/obj/%.asm.o",$(OBJS))

#End

# target      _welcome dependencies
all:	_welcome chkcross clean directories compile link libs sysroot initrd iso run
	@echo -e "$(ARROW) U365: done\e[0m"

# Updates libs and sysroot and runs new system, without recompiling the kernel
refill: _welcome directories libs sysroot initrd iso run
	@echo -e
	
chkcross:
	@./chkcross.sh $(CROSSBIN) $(ARCH)-$(BINFORMAT)-
run:          _welcome
	@echo -e " $(ARROW) Booting the ISO image\e[0m"
	$(EMU) $(EFLAGS) &> /dev/null
	@echo -n
# Welcome user at make call
_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32mU365\e[0m"

compile:     _welcome clean directories _compile $(SOURCES)
	@echo -n

libs:        _welcome directories
	@echo -e " $(ARROW) Making libs\e[0m"
	@cd initrd/usr/src/libs && make

sysroot:     _welcome directories libs
	@echo -e " $(ARROW) Making sysroot\e[0m"
	@cd initrd/usr/src/apps && make

initrd:      _welcome directories
	@echo -e " $(ARROW) Generating initrd\e[0m"
	@cd initrd; tar -cf ../build/iso/fs/boot/initrd.tar *

link:        _welcome clean directories compile
	@echo -e " $(ARROW) Linking\e[0m"
	@$(CC) $(OBJS) $(LDFLAGS)

iso:         _welcome directories
	@echo -e " $(ARROW) Generating an ISO image\e[0m"

	@echo "insmod gfxterm;  \
insmod vbe;  \
timeout=5;   \
set gfxmode=1024x768;  \
menuentry \"U365\"  \
{  \
	echo 'Loading kernel...';            \
	multiboot /boot/u365.elf;            \
	echo 'Loading initrd...'            ;\
	module    /boot/initrd.tar  initrd;  \
	module    /boot/test        elf;\
	boot;  \
}  \
" > $(BUILD_DIR)/iso/fs/grub.cfg

	@cp $(BUILD_DIR)/iso/fs/grub.cfg $(BUILD_DIR)/iso/fs/boot/grub/grub.cfg
	@cp $(BUILD_DIR)/bin/$(BINFILE).$(BINFORMAT) $(BUILD_DIR)/iso/fs/boot/
	@grub-mkrescue $(MKRSCFLAGS) &> /dev/null
	@echo -n

clean:       _welcome
	@echo -e " $(ARROW) Cleaning\e[0m"
	@rm -rf $(BUILD_DIR) *.iso initrd.tar
	@cd initrd/usr/src/libs && make clean
	@cd initrd/usr/src/apps && make clean
	@echo -n

directories: _welcome
	@echo -e " $(ARROW) Creating build directories\e[0m"
	@mkdir -p $(OBJDIRS) $(CFSDIRS) $(BUILD_DIR)/bin $(BUILD_DIR)/iso/fs/boot $(BUILD_DIR)/iso/fs/boot/grub $(BUILD_DIR)/iso/fs/fonts $(BUILD_DIR)/releases
	@echo -n

Makefile:
	@echo -e " \e[1;31mStrange make bug prevented\e[0m"

# Compilation notification - do not remove
_compile:
	@echo -e " $(ARROW) Compiling\e[0m"
# Compilation routines
%.c:          _welcome directories _compile
	@echo -e " \e[0;32m Building C   file:[0m \e[1;32m$@\e[0m"
	@$(CC) $@ -o $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/obj/%.c.o,$@) $(CFLAGS)
	@echo -n

%.cpp:          _welcome directories _compile
	@echo -e " \e[0;32m Building C++ file:\e[0m \e[1;32m$@\e[0m"
	@$(CXX) $@ -o $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/obj/%.cpp.o,$@) $(CXXFLAGS)
	@echo -n

%.s:          _welcome directories _compile
	@echo -e " \e[0;32m Building GAS file:\e[0m \e[1;32m$@\e[0m"
	@$(GAS) $@ -o $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/obj/%.s.o,$@)
	@echo -n

%.asm:        _welcome directories _compile
	@echo -e " \e[0;32m Building IAS file:\e[0m \e[1;32m$@\e[0m"
	@$(IASM) $@ -o $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/obj/%.asm.o,$@) $(IASFLAGS)
	@echo -n
