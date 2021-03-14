# cinit
Create a file structure and Makefile for your C projects.
The Makefile defaults to c99 and clang.

## Usage
Compile the program with `make` and invoke it with the following command:

```sh
./cinit <project-name>
```

## Structure
This command will create the following structure:
```
<project-name>/
├─ Makefile
├─ src/
│  ├─ main.c
├─ include/
```

## File content
The files on the project will have the following content:

### `main.c`
```c
int main(int argc, char *argv[]) {
    return 0;
}
```

### `Makefile`
```make
comp=clang
src=src/*.c
incl=-Iinclude
out=<project-name>
libs=
std=-std=c99

all:
        @$(comp) -o $(out) $(src) $(incl) $(libs) $(std)

debug:
        @$(comp) -o $(out) $(src) $(incl) $(libs) $(std) -g

run:
        @./$(out)
```
