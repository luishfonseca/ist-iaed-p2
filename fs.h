/*
 * File:	fs.h
 * Author:	Luís Fonseca, 99266
 * Desc:	This header exposes the filesystem interface.
 */

#define FS_ROOT "/"
#define PATH_DELIMITER "/"

#define OK 0
#define ERR_NOT_FOUND 1
#define ERR_NO_DATA 2
#define ERR_NO_MEMORY 3

struct FS;

struct FS* fs_init();
int fs_set(struct FS* fs, char* path, char* value);
int fs_remove(struct FS* fs, char* path);
int fs_find(struct FS* fs, char* path);
int fs_list(struct FS* fs, char* path);
int fs_search(struct FS* fs, char* value);
int fs_print(struct FS* fs);
