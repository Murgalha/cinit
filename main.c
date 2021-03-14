#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

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
		exit(1);
	}

	fprintf(fp, "%s", content);

	fclose(fp);
	printf("'%s' written\n", filename);
}

void create_makefile() {
	char out[256] = {0};

	// copy the name of the project to the 'out=' line
	strcpy(out, "out=");
	strcat(out, project_name);
	strcat(out, "\n");

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
	int total_len = strlen(out) + strlen(makefile_str1) + strlen(makefile_str2);

	// Alloc and copy Makefile content to a single char *
	char *makefile_content = calloc(total_len + 1, sizeof(char));
	strcat(makefile_content, makefile_str1);
	strcat(makefile_content, out);
	strcat(makefile_content, makefile_str2);
	makefile_content[total_len] = '\0';
	create_file("Makefile", makefile_content);

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
			if(!strcmp(directory->d_name, ".") && !strcmp(directory->d_name, "..")) {
				printf("Directory not empty! Please select a new or empty directory");
				exit(1);
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

	return 0;
}
