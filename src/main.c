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

char *empty_main = {
	"int main(int argc, char *argv[]) {\n"
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
	char *out = mtbs_join(3, "out=", project_name, "\n");
	char *std = mtbs_join(3, "std=-std=", (cpp_flag ? "c++98" : "c99"), "\n\n");
	char *src = mtbs_join(3, "src=src/*", extension, "\n");

	// because we need to concatenate the Makefile content with
	// the name of the project, the file string is split in 2
	char *compiler_str = mtbs_join(3, "comp=", compiler, "\n");
	char *makefile_str1 = {
		"incl=-Iinclude\n"
	};

	char *makefile_str2 = {
		"libs=\n"
	};
	char *makefile_str3 = {
		"all:\n"
		"\t@$(comp) -o $(out) $(src) $(incl) $(libs) $(std)\n\n"
		"debug:\n"
		"\t@$(comp) -o $(out) $(src) $(incl) $(libs) $(std) -g\n\n"
		"run:\n"
		"\t@./$(out)\n"
	};

	char *makefile_content = mtbs_join(7, compiler_str, src, makefile_str1, out, makefile_str2,
									   std, makefile_str3);
	create_file("Makefile", makefile_content);

	free(compiler_str);
	free(out);
	free(std);
	free(src);
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
	create_file(main_filename, empty_main);
	free(main_filename);

	free_globals();
	return EXIT_SUCCESS;
}
