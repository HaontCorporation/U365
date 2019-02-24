ARROW=\e[1;34m==>\e[1;37m

all: _welcome clean compile
	@echo -e "$(ARROW) sysroot: done\e[0m"

_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32mU365 sysroot\e[0m"

_compile:
	@echo -e " $(ARROW) Compiling\e[0m"

compile: _welcome clean _compile
	@find ./ -name "Makefile" -and -not -path "./Makefile" -execdir make \;

clean:
	@echo -e " $(ARROW) Cleaning\e[0m"
	@find ./ -name "Makefile" -and -not -path "./Makefile" -execdir make clean \;
