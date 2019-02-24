ARROW=\e[1;34m==>\e[1;37m

TARGET="U365 libs"

all: _welcome clean compile
	@echo -e "$(ARROW) [$(TARGET)]: done\e[0m"

_welcome:
	@echo -e " \e[1;33mMakefile:\e[0m \e[1;32m$(TARGET)\e[0m"

_compile:
	@echo -e " $(ARROW) [$(TARGET)]: Compiling\e[0m"

compile: _welcome clean _compile
	@find ./ -name "Makefile" -and -not -path "./Makefile" -execdir make \;

clean:
	@echo -e " $(ARROW) [$(TARGET)]: Cleaning\e[0m"
	@find ./ -name "Makefile" -and -not -path "./Makefile" -execdir make clean \;
