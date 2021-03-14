comp=clang
src=*.c
out=cinit
std=-std=c99

all:
	@$(comp) -o $(out) $(src) $(std)

run:
	@echo "Usage: ./$(out) <dir-name>"
