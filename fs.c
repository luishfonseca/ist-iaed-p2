#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

struct File {
	char* path;
	char* value;
};

struct Directory {
	int id;
	char* rel_path;
	struct File* f;
	struct AVL* subdirs_by_id;
	struct AVL* subdirs_by_path;
};

struct FS {
	struct Directory* root;
};

char* strdup(char* str) {
	return strcpy(malloc(strlen(str)+1),str);
}

struct File* new_file(char* path, char* value) {
	struct File* f = malloc(sizeof(struct Directory));
	/* Clean up path before setting it */
	f->path = strdup(path);
	f->value = strdup(value);
	return f;
}

void remove_file(struct File* f) {
	free(f->path);
	free(f->value);
	free(f);
}

void print_file(struct File* f) {
	printf("%s %s\n", f->path, f->value);
}

void print_file_value(struct File* f) {
	printf("%s\n", f->value);
}

void print_file_path(struct File* f) {
	printf("%s\n", f->path);
}

struct Directory* new_directory(char* rel_path) {
	static int id = 0;
	struct Directory* dir = malloc(sizeof(struct Directory));
	dir->id = id++;
	dir->rel_path = strdup(rel_path);
	dir->f = NULL;
	dir->subdirs_by_id = NULL;
	dir->subdirs_by_path = NULL;
	return dir;
}

int search_path(void* path, void* dir) {
	char* path1 = (char*)path;
	char* path2 = ((struct Directory*)dir)->rel_path;
	return strcmp(path1, path2);
}

int cmp_paths(void* dir1, void* dir2) {
	char* path1 = ((struct Directory*)dir1)->rel_path;
	char* path2 = ((struct Directory*)dir2)->rel_path;
	return strcmp(path1, path2);
}

int cmp_ids(void* dir1, void* dir2) {
	int id1 = ((struct Directory*) dir1)->id;
	int id2 = ((struct Directory*) dir2)->id;
	return id1 - id2;
}

void print_dir_relative_path(void* d) {
	struct Directory* dir = d;
	printf("%s\n", dir->rel_path);
}

struct Directory* aux_create_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path = strtok(path, "/");

	if (rel_path == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL) {
		sub = new_directory(rel_path);
		dir->subdirs_by_id = avl_insert(dir->subdirs_by_id, sub, cmp_ids);
		dir->subdirs_by_path = avl_insert(dir->subdirs_by_path, sub, cmp_paths);
	}

	return aux_create_directory(sub, NULL);
}

struct Directory* create_directory(struct Directory* root, char* path) {
	char* p = strdup(path);
	struct Directory* dir = aux_create_directory(root, p);
	free(p);
	return dir;
}

struct Directory* aux_find_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path = strtok(path, "/");

	if (rel_path == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL)
		return NULL;
	else
		return aux_find_directory(sub, NULL);
}

struct Directory* find_directory(struct Directory* root, char* path) {
	char* p = strdup(path);
	struct Directory* dir = aux_find_directory(root, p);
	free(p);
	return dir;
}

void print_all(void* d) {
	struct Directory* dir = d;

	if (dir != NULL) {
		if (dir->f != NULL)
			print_file(dir->f);

		avl_traverse(dir->subdirs_by_id, print_all);
	}
}

struct FS* fs_init() {
	struct FS* fs = malloc(sizeof(struct FS));
	fs->root = new_directory("/");
	return fs;
}

int fs_set(struct FS* fs, char* path, char* value) {
	struct File* f = new_file(path, value);
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		dir = create_directory(fs->root, path);

	if (dir->f != NULL)
		remove_file(dir->f);
	dir->f = f;

	return 0;
}

int fs_find(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return 1; /* Not found */
	else if (dir->f == NULL)
		return 2; /* No data */
	else
		print_file_value(dir->f);

	return 0;
}

int fs_list(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return 1; /* Not found */
	else
		avl_traverse(dir->subdirs_by_path, print_dir_relative_path);

	return 0;
}

int fs_print(struct FS* fs) {
	print_all(fs->root);
	return 0;
}
