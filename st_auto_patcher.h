#pragma once

#include "types.h"

#define SUCCESS 0;
#define NEED_HELP 1;
#define NOT_FOUND 404;
#define BACKUP_FAILED 101;
#define NO_PATCHES 111;
#define CANT_PATCH 123;

#define BACKUP_EXTENSION ".bak"

#define print_help(void) fprintf(stderr, "usage: st_auto_patcher [sublime_text]\n");


int main(int, char *[]);

bool is_writable(char *);

bool make_backup(char *, char *);

bool patch_executable(char *, patch);
