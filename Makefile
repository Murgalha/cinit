comp=gcc
src=src/*.c
incl=-Iinclude
out=cinit
std=-std=c99

all:
	@$(comp) -o $(out) $(src) $(incl) $(std)

run:
	@echo "Usage: ./$(out) <project-name>"
