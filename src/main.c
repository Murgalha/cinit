#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#define MTB_STR_IMPLEMENTATION
#include "mtb_str.h"

char project_name[100] = {0};

char *empty_main = {
	"int main(int argc, char *argv[]) {\n"
	"\treturn 0;\n"
	"}\n"
};

void create_file(char *filename, char *content) {
	FILE *fp = fopen(filename, "w");
	if(!fp) {
		printf("Could not create '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%s", content);

	fclose(fp);
	printf("'%s' written\n", filename);
}

void create_makefile() {
	char *out = mtbs_join(3, "out=", project_name, "\n");

	// because we need to concatenate the Makefile content with
	// the name of the project, the file string is split in 2
	char *makefile_str1 = {
		"comp=clang\n"
		"src=src/*.c\n"
		"incl=-Iinclude\n"
	};

	char *makefile_str2 = {
		"libs=\n"
		"std=-std=c99\n\n"
		"all:\n"
		"\t@$(comp) -o $(out) $(src) $(incl) $(libs) $(std)\n\n"
		"debug:\n"
		"\t@$(comp) -o $(out) $(src) $(incl) $(libs) $(std) -g\n\n"
		"run:\n"
		"\t@./$(out)\n"
	};

	char *makefile_content = mtbs_join(3, makefile_str1, out, makefile_str2);
	create_file("Makefile", makefile_content);

	free(out);
	free(makefile_content);
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("No directory name provided!\n");
		printf("Usage: %s <project-name>\n", argv[0]);
		exit(1);
	}
	else if(argc > 2) {
		printf("Invalid number of arguments. Please provide only one!\n");
		printf("Usage: %s <project-name>\n", argv[0]);
		exit(1);
	}

	strcpy(project_name, argv[1]);
	DIR *d;
	struct dirent *directory;

	d = opendir(project_name);
	if (d) {
		while((directory = readdir(d))) {
			if(!strcmp(directory->d_name, ".") || !strcmp(directory->d_name, "..")) {
				printf("Directory not empty!\nPlease select a new or empty directory\n");
				closedir(d);
				exit(EXIT_FAILURE);
			}
		}
    }
	else
		mkdir(project_name, 0777);

	chdir(project_name);
	// Create directories
	mkdir("src", 0777);
	mkdir("include", 0777);
	// Create files
	create_makefile();
	create_file("src/main.c", empty_main);

	return EXIT_SUCCESS;
}
