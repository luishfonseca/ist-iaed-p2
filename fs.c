#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

struct Directory {
	int id;
	char* path;
	char* value;
	struct Directory* p;
	struct AVL* subdirs_by_id;
	struct AVL* subdirs_by_path;
};

struct FS {
	struct Directory* root;
};

char* strdup(char* str) {
	return strcpy(malloc(strlen(str)+1),str);
}

struct Directory* new_directory(char* rel_path) {
	static int id = 0;
	struct Directory* dir = malloc(sizeof(struct Directory));
	dir->id = id++;
	dir->path = strdup(rel_path);
	dir->value = NULL;
	dir->p = NULL;
	dir->subdirs_by_id = NULL;
	dir->subdirs_by_path = NULL;
	return dir;
}

int search_path(void* path, void* dir) {
	char* path1 = (char*)path;
	char* path2 = ((struct Directory*)dir)->path;
	return strcmp(path1, path2);
}

int cmp_paths(void* dir1, void* dir2) {
	char* path1 = ((struct Directory*)dir1)->path;
	char* path2 = ((struct Directory*)dir2)->path;
	return strcmp(path1, path2);
}

int cmp_ids(void* dir1, void* dir2) {
	int id1 = ((struct Directory*) dir1)->id;
	int id2 = ((struct Directory*) dir2)->id;
	return id1 - id2;
}

void print_dir_relative_path(void* d) {
	struct Directory* dir = d;
	printf("%s\n", dir->path);
}

void print_dir_full_path(void* d) {
	struct Directory* dir = d;
	if (dir->p != NULL && strcmp(dir->p->path, "/") != 0)
		print_dir_full_path(dir->p);
	printf("/%s", dir->path);
}

struct Directory* create_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path = strtok(path, "/");

	if (rel_path == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL) {
		sub = new_directory(rel_path);
		sub->p = dir;
		dir->subdirs_by_id = avl_insert(dir->subdirs_by_id, sub, cmp_ids);
		dir->subdirs_by_path = avl_insert(dir->subdirs_by_path, sub, cmp_paths);
	}

	return create_directory(sub, NULL);
}

struct Directory* find_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path;

	if (dir == NULL)
		return NULL;

	if ((rel_path = strtok(path, "/")) == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL)
		return NULL;
	else
		return find_directory(sub, NULL);
}

void remove_directory(void* d) {
	struct Directory* dir = d;

	if (dir != NULL) {
		if (dir->value != NULL)
			free(dir->value);

		avl_destroy(dir->subdirs_by_path, remove_directory);
		avl_destroy(dir->subdirs_by_id, NULL);

		free(dir->path);
		free(dir);
	}
}

void print_all(void* d) {
	struct Directory* dir = d;

	if (dir != NULL) {
		if (dir->value != NULL) {
			print_dir_full_path(dir);
			printf(" %s\n", dir->value);
		}

		avl_traverse(dir->subdirs_by_id, print_all);
	}
}

struct FS* fs_init() {
	struct FS* fs = malloc(sizeof(struct FS));
	fs->root = NULL;
	return fs;
}

int fs_set(struct FS* fs, char* path, char* value) {
	struct Directory* dir;

	if (fs->root == NULL)
		fs->root = new_directory("/");

	dir = create_directory(fs->root, path);

	if (dir->value != NULL)
		free(dir->value);
	dir->value = strdup(value);

	return 0;
}

int fs_find(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return 1; /* Not found */
	else if (dir->value == NULL)
		return 2; /* No data */
	else
		printf("%s\n", dir->value);

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

int fs_remove(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return 1; /* Not found */
	else {
		if (dir->p != NULL) {
			dir->p->subdirs_by_path =
				avl_remove(dir->p->subdirs_by_path, dir, cmp_paths);
			dir->p->subdirs_by_id =
				avl_remove(dir->p->subdirs_by_id, dir, cmp_ids);
		}
		remove_directory(dir);
	}

	if (dir == fs->root)
		fs->root = NULL;

	return 0;
}

int fs_print(struct FS* fs) {
	print_all(fs->root);
	return 0;
}
