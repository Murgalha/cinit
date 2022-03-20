# cinit
Create a file structure and Makefile for your C/C++ projects.

## Usage
Compile the program with `make` and invoke it with the following command:

```sh
cinit <project-name>
```

### Command line options
You can use command line options with cinit. The options available are:
```sh
-c, --compiler <compiler> # Sets the compiler to use on the Makefile
# Important: The variable used is $CC or $CXX (for C and C++, respectively), therefore the compiler passed as
# command line option will only be used if the variable used is not set.

--cpp # Sets generated file extensions to be .cpp and compiler to g++

# Usage:
cinit [-c <compiler>] [--cpp] <project-name>
```
Note: The compiler will simply be copied to the Makefile without any checks. Make sure you have installed
and the executable is in the `PATH` variable.

## Structure
This command will create the following structure:
```
<project-name>/
├─ Makefile
├─ src/
│  ├─ main.c(pp)
├─ include/
```

## File content
The files on the project will have the following content, assuming C project.
For the C++ one, the changes made are as stated above:

### `main.c`
```c
#include <stdio.h>

int main(int argc, char *argv[]) {
	printf("Args: %d\n", argc);

	int i;
	for(i = 0; i < argc; i++) {
		printf("Arg #%d: %s\n", i, argv[i]);
	}

	return 0;
}
```

### `Makefile`
```make
CC ?= <compiler>
SRCDIR=src/
INCLUDEDIR=include/
WARNFLAGS=-Wall -Wextra -Werror
STD=-std=<std>

WRKDIR=build/
OBJDIR := ${WRKDIR}obj/
HEADERFILES := $(wildcard ${INCLUDEDIR}*.h)
SRCFILES := $(wildcard ${SRCDIR}*.c)
OBJFILES := ${addprefix ${OBJDIR}, ${notdir ${SRCFILES:.c=.o}}}

# EXECUTABLE STUFF
BIN=<project-name>
BINDIR := ${WRKDIR}bin/
BINFILE := ${BINDIR}${BIN}

all: prepare ${BINFILE}

${OBJDIR}%.o: ${SRCDIR}%.c ${HEADERFILES}
	$(CC) -c $< ${WARNFLAGS} -I${INCLUDEDIR} -o $@ ${STD}

${BINFILE}: ${OBJFILES}
	$(CC) $^ ${WARNFLAGS} -I${INCLUDEDIR} -o $@ ${STD}

run:
	@./${BINFILE}

prepare:
	@if [ ! -d "${WRKDIR}" ]; then mkdir ${WRKDIR}; fi
	@if [ ! -d "${OBJDIR}" ]; then mkdir ${OBJDIR}; fi
	@if [ ! -d "${BINDIR}" ]; then mkdir ${BINDIR}; fi

clear:
	rm -rf ${WRKDIR}
```
