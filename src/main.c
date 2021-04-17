#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#define MTB_STR_IMPLEMENTATION
#include "mtb_str.h"

char *project_name = NULL;
char *compiler = NULL;

char *empty_main = {
	"int main(int argc, char *argv[]) {\n"
	"\treturn 0;\n"
	"}\n"
};

void free_globals() {
	free(project_name);
	free(compiler);
}

void create_file(char *filename, char *content) {
	FILE *fp = fopen(filename, "w");
	if(!fp) {
		printf("Could not create '%s'\n", filename);
		free_globals();
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
	char *compiler_str = mtbs_join(3, "comp=", compiler, "\n");
	char *makefile_str1 = {
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

	char *makefile_content = mtbs_join(4, compiler_str, makefile_str1, out, makefile_str2);
	create_file("Makefile", makefile_content);

	free(compiler_str);
	free(out);
	free(makefile_content);
}

int main(int argc, char *argv[]) {
	int opt;
	while((opt = getopt(argc, argv, "c:")) != -1) {
        switch(opt) {
		case 'c':
			compiler = mtbs_new(optarg);
			break;
		case ':':
			printf("option needs a value\n");
			exit(EXIT_FAILURE);
			break;
		case '?':
			// TODO: Show help
			free_globals();
			exit(EXIT_FAILURE);
			break;
        }
    }
	if(!compiler) {
		compiler = mtbs_new("gcc");
	}

	if(optind != argc-1) {
		// TODO: Show help
		printf("Invalid arguments! Must provide project name\n");
		free_globals();
		exit(EXIT_FAILURE);
	}
	project_name = mtbs_new(argv[optind]);

	DIR *d;
	struct dirent *directory;

	d = opendir(project_name);
	if (d) {
		while((directory = readdir(d))) {
			if(!strcmp(directory->d_name, ".") || !strcmp(directory->d_name, "..")) {
				printf("Directory not empty!\nPlease select a new or empty directory\n");
				closedir(d);
				free_globals();
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

	free_globals();
	return EXIT_SUCCESS;
}
