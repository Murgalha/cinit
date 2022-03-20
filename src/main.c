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
char *project_path = NULL;
char *compiler = NULL;
char *extension = NULL;
int cpp_flag = 0;

char *main_content = {
	"#include <stdio.h>\n\n"
	"int main(int argc, char *argv[]) {\n"
	"\tprintf(\"Args: %d\\n\", argc);\n\n"
	"\tint i;\n"
	"\tfor(i = 0; i < argc; i++) {\n"
		"\t\tprintf(\"Arg #%d: %s\\n\", i, argv[i]);\n"
	"\t}\n"
	"\treturn 0;\n"
	"}\n"
};

void free_globals() {
	free(project_name);
	free(project_path);
	free(compiler);
	free(extension);
}

char *basename(char *path) {
	int n;
	char **tokens = mtbs_split(path, &n, "/");
	char *base = mtbs_new(tokens[n-1]);
	mtbs_free_split(tokens, n);
	return base;
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
	// because we need to concatenate the Makefile content with
	// the name of the project, the file string is split in 2
	char *compiler_var = (cpp_flag ? "CXX" : "CC");
	char *compiler_str = mtbs_join(4, compiler_var, " ?= ", compiler, "\n");
	char *std = mtbs_join(3, "STD=-std=", (cpp_flag ? "c++98" : "c99"), "\n\n");

	char *comp_str = {
		"SRCDIR=src/\n"
		"INCLUDEDIR=include/\n"
		"WARNFLAGS=-Wall -Wextra -Werror\n"
		"LIBS=\n"
	};

	char *wrkdir_str = {
		"WRKDIR=build/\n"
		"OBJDIR := ${WRKDIR}obj/\n"
		"HEADERFILES := $(wildcard ${INCLUDEDIR}*.h)\n"
	};

	char *src_files = mtbs_join(3, "SRCFILES := $(wildcard ${SRCDIR}*", extension, ")\n");
	char *obj_files = mtbs_join(3, "OBJFILES := ${addprefix ${OBJDIR}, ${notdir ${SRCFILES:", extension, "=.o}}}\n\n");
	char *bin = mtbs_join(3, "# EXECUTABLE STUFF\nBIN=", project_name, "\n");

	char *content_1 = {
		"BINDIR := ${WRKDIR}bin/\n"
		"BINFILE := ${BINDIR}${BIN}\n\n"
		"all: prepare ${BINFILE}\n\n"
	};
	char *obj_target = mtbs_join(3, "${OBJDIR}%.o: ${SRCDIR}%", extension, " ${HEADERFILES}\n");

	char *compilation_string = mtbs_join(3, "\t$(", compiler_var, ") -c $< ${WARNFLAGS} -I${INCLUDEDIR} -o $@ ${STD}\n\n");
	char *exe_creation_string = mtbs_join(3, "${BINFILE}: ${OBJFILES}\n\t$(", compiler_var, ") $^ ${WARNFLAGS} -I${INCLUDEDIR} -o $@ ${STD} ${LIBS}\n\n");

	char *content_2 = {
		"run:\n"
		"\t@./${BINFILE}\n\n"

		"prepare:\n"
		"\t@if [ ! -d \"${WRKDIR}\" ]; then mkdir ${WRKDIR}; fi\n"
		"\t@if [ ! -d \"${OBJDIR}\" ]; then mkdir ${OBJDIR}; fi\n"
		"\t@if [ ! -d \"${BINDIR}\" ]; then mkdir ${BINDIR}; fi\n\n"

		"clear:\n"
		"\trm -rf ${WRKDIR}\n"
	};

	char *makefile_content = mtbs_join(12, compiler_str, comp_str, std, wrkdir_str,
									   src_files, obj_files, bin, content_1, obj_target, compilation_string, exe_creation_string, content_2);
	create_file("Makefile", makefile_content);

	free(compiler_str);
	free(std);
	free(src_files);
	free(obj_files);
	free(bin);
	free(makefile_content);
}

int main(int argc, char *argv[]) {
	int opt, indexptr;
	struct option long_options[] = {
		{"compiler", required_argument, 0, 'c'},
		{"cpp", no_argument, &cpp_flag, 1},
		{0, 0, 0, 0}
	};

	while((opt = getopt_long(argc, argv, "c:", long_options, &indexptr)) != -1) {
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

	if(optind != argc-1) {
		// TODO: Show help
		printf("Invalid arguments! Must provide project name\n");
		free_globals();
		exit(EXIT_FAILURE);
	}
	if(!compiler) {
		if(cpp_flag)
			compiler = mtbs_new("g++");
		else
			compiler = mtbs_new("gcc");
	}

	project_path = mtbs_new(argv[optind]);
	project_name = basename(argv[optind]);

	if(cpp_flag) {
		extension = mtbs_new(".cpp");
	}
	else {
		extension = mtbs_new(".c");
	}

	DIR *d;
	struct dirent *directory;

	d = opendir(project_path);
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
		mkdir(project_path, 0777);

	chdir(project_path);
	// Create directories
	mkdir("src", 0777);
	mkdir("include", 0777);
	// Create files
	create_makefile();

	char *main_filename = mtbs_join(2, "src/main", extension);
	create_file(main_filename, main_content);
	free(main_filename);

	free_globals();
	return EXIT_SUCCESS;
}
