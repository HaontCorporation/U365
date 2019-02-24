ARROW=\e[1;34m==>\e[1;37m

ARCH       = i686
BINFORMAT  = elf
BITS       = 32
CROSS      = ~/opt/cross
CROSSBIN   = $(CROSS)/bin/
GAS        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-as #AT&T-syntax assembler
CC         = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-gcc
CXX        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-g++
CFLAGS     = -m32 -O3 -ffreestanding -Wall -Wextra -Wmaybe-uninitialized -nostdlib -std=gnu11 -I../../../../usr/include -L../../../../lib/ -lc
#SOURCES    = $(shell find . -name "*.c" -type f -print)
BIN        = hello

all: _welcome clean compile install
	@echo -e " $(ARROW) $(BIN): done\e[0m"

_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32mU365 $(BIN)\e[0m"

_compile:
	@echo -e " $(ARROW) Compiling\e[0m"

clean:
	@echo -e " $(ARROW) Cleaning\e[0m"
	@rm -f *.o $(BIN)

compile: _welcome clean _compile
	gcc *.c -o $(BIN) $(CFLAGS)

install: _welcome compile
	@echo -e " $(ARROW) Installing\e[0m"
	@cp $(BIN) ../../../../bin/$(BIN)
