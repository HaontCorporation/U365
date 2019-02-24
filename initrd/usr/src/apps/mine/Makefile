ARROW=\e[1;34m==>\e[1;37m

TARGET     = mine
ROOT       = ../../../..
INSTALL    = $(ROOT)/bin
ARCH       = i686
BINFORMAT  = elf
BITS       = 32
CROSS      = ~/opt/cross
CROSSBIN   = $(CROSS)/bin/
GAS        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-as #AT&T-syntax assembler
CC         = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-gcc
CXX        = $(CROSSBIN)$(ARCH)-$(BINFORMAT)-g++
CFLAGS     = -m32 -O2 -ffreestanding -Wall -Wextra -Wmaybe-uninitialized -nostdlib -std=gnu11 -I$(ROOT)/usr/include -L$(ROOT)/lib/ -lhwidgets -lextra -lsurface -lm -lc
CXXFLAGS   = -m32 -O2 -ffreestanding -Wall -Wextra -Wmaybe-uninitialized -nostdlib -std=c++11 -I$(ROOT)/usr/include -L$(ROOT)/lib/ -lhwidgets -lextra -lsurface -lm -lc -lcpp -fno-exceptions -fno-rtti
#SOURCES    = $(shell find . -name "*.c" -type f -print)

all: _welcome clean compile install
	@echo -e " $(ARROW) [$(TARGET)]: done\e[0m"

_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32mU365 $(TARGET)\e[0m"

_compile:
	@echo -e " $(ARROW) [$(TARGET)]: Compiling\e[0m"

clean:
	@echo -e " $(ARROW) [$(TARGET)]: Cleaning\e[0m"
	@rm -f $(INSTALL)/$(TARGET) *.o $(TARGET)

compile: _welcome clean _compile
	g++ *.cpp -o $(TARGET) $(CXXFLAGS)

install: _welcome compile
	@echo -e " $(ARROW) [$(TARGET)]: Installing\e[0m"
	@cp $(TARGET) $(INSTALL)/$(TARGET)

directories: _welcome
	@mkdir -p $(INSTALL)
