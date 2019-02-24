ARROW=\e[1;34m==>\e[1;37m

TARGET     = libc
ROOT       = ../../../..
INSTALL    = $(ROOT)/lib
ARCH       = i686
BINFORMAT  = elf
BITS       = 32
CROSS      = ~/opt/cross
CROSSBIN   = $(CROSS)/bin/
GAS        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-as #AT&T-syntax assembler
CC         = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-gcc
CXX        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-g++
CFLAGS     = -m32 -O2 -ffreestanding -Wall -Wextra -Wmaybe-uninitialized -nostdlib -std=gnu11 -I$(ROOT)/usr/include -lgcc
#SOURCES    = $(shell find . -name "*.c" -type f -print)

all:      _welcome clean compile link install
	@echo -e " $(ARROW) [$(TARGET)]: done\e[0m"

_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32mU365 $(TARGET)\e[0m"

_compile: _welcome
	@echo -e " $(ARROW) [$(TARGET)]: Compiling\e[0m"

clean:    _welcome
	@echo -e " $(ARROW) [$(TARGET)]: Cleaning\e[0m"
	@rm -f *.o *.a $(INSTALL)/$(TARGET).a

compile:  _welcome clean _compile #$(SOURCES)
	gcc -c *.c $(CFLAGS)

link:     _welcome clean
	@echo -e " $(ARROW) [$(TARGET)]: Linking\e[0m"
	@ar rcs $(TARGET).a *.o

#%.c:          _welcome directories _compile
#	@echo -e " \e[0;32m Building C   file:[0m \e[1;32m$@\e[0m"
#	@$(CC) $@ -o $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/obj/%.c.o,$@) $(CFLAGS)
#	@echo -n

install: _welcome directories
	@echo -e " $(ARROW) [$(TARGET)]: Installing\e[0m"
	@cp $(TARGET).a $(INSTALL)/$(TARGET).a
	
directories: _welcome
	@mkdir -p $(INSTALL)
