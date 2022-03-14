#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "patch_finder.h"
#include "path_finder.h"
#include "st_auto_patcher.h"
#include "types.h"


int main(int argc, char *argv[]) {
	char *backup_path = NULL;
	char *path = NULL;
	patch *patches = NULL;
	int patch_index = 0;

	// display help dialog
	if (argc == 2) {
		if ((argv[1] == "-h") | (argv[1] == "--help")) {
			print_help();
			return NEED_HELP;
		}
	}
	else if (argc > 2) {
		print_help();
		return NEED_HELP;
	}

	/*
	try to get executable path from args
	malloc: path
	*/
	if (argc == 2) {
		path = malloc(strlen(argv[1]));
		strcpy(path, argv[1]);
	}
	// otherwise try to find path ourselves
	else {
		path = find_path();
	}
	// quit if no executable is found
	if (!(path)) {
		return NOT_FOUND;
	}

	/*
	make a backup of the executable before modifying it
	malloc: backup_path
	*/
	backup_path = malloc(strlen(path)+strlen(BACKUP_EXTENSION));
	strcpy(backup_path, path);
	strcat(backup_path, BACKUP_EXTENSION);
	if (!(make_backup(path, backup_path))) {
		fprintf(stderr, "can't create backup, missing file permissions for %s/%s ?\n", path, BACKUP_EXTENSION);
		return BACKUP_FAILED;
	}
	printf("made backup: %s.bak\n", path);

	/* 
	begin patch finding 
	malloc: patches
	*/
	printf("finding patches...\n"); 
	if (!(patches = find_patches(path))) {
		return NO_PATCHES;
	}

	// apply all patches and revert to backup upon failure
	printf("patching executable...\n"); 
	for (patch_index; patch_index < (sizeof(patches) / sizeof(patches[0])); patch_index + sizeof(patches[0])); {
		if (!(patch_executable(path, patches[patch_index]))) {
			// make_backup() also functions to restore from a backup
			if (!(make_backup(backup_path, path))) {
				fprintf(stderr, "can't restore from backup, missing file permissions for %s/%s ?\n", path, BACKUP_EXTENSION);
			}
			fprintf(stderr, "patching failed\n");
			return CANT_PATCH;
		}
	}

	/*
	clean up and leave
	free: path, backup_path, patches
	*/
	printf("done\n");
	free(path);
	free(backup_path);
	free(patches);
	return SUCCESS;
}

bool is_writable(char *path) {
	FILE *fd; int writable = false;

	if (fd = fopen(path, "r+")) {
		writable = true;
	}

	fclose(fd);
	return writable;
}

bool make_backup(char *path, char *backup_path) {
	FILE *st;
	FILE *backup;
	char c;

	if ((!(st = fopen(path, "r"))) | (!(backup = fopen(backup_path, "w")))) {
		return false;
	}
	while ((c = fgetc(st)) != EOF) {
		fputc(c, backup);
	}

	fclose(st);
	fclose(backup);
	return true;
}

bool patch_executable(char *path, patch p) {
	FILE *fd = fopen(path, "r+");

	// sanity check
	if (!(fseek(fd, p.offset + p.size, SEEK_SET))) {
		fprintf(stderr, "patch too large: [(%i) @ (%li)]\n", p.value, p.offset); 
		return false;
	}
	printf("file sanity check okay\n");

	// patch executable
	fseek(fd, p.offset, SEEK_SET);
	if (fputs(p.value, fd) == EOF) {
		printf("write failure: [(%i) @ (%li)]\n", p.value, p.offset);
		return false;
	}

	printf("wrote: (%i) @ (%li)\n", p.value, p.offset);
	fclose(fd);
	return true;
}
