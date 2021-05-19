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

char* clean_path(char* path) {
	char* p = strdup(path);
	char buffer[65536] = "";
	char* token = strtok(p, "/");

	while (token != NULL) {
		strncat(buffer, "/", 2);
		strncat(buffer, token, strlen(token));
		token = strtok(NULL, "/");
	}

	free(p);
	return strdup(buffer);
}

struct File* new_file(char* path, char* value) {
	struct File* f = malloc(sizeof(struct Directory));
	f->path = clean_path(path);
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
	dir->p = NULL;
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
	char* rel_path = strtok(path, "/");

	if (rel_path == NULL)
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
		if (dir->f != NULL)
			remove_file(dir->f);

		avl_destroy(dir->subdirs_by_path, remove_directory);
		avl_destroy(dir->subdirs_by_id, NULL);

		free(dir->rel_path);
		free(dir);
	}
}

void prune_branch(struct Directory* dir) {
	struct Directory* aux;

	while (dir->subdirs_by_path == NULL && dir->f == NULL) {
		aux = dir->p;

		aux->subdirs_by_path = avl_remove(aux->subdirs_by_path, dir, cmp_paths);
		free(dir->subdirs_by_path);
		aux->subdirs_by_id = avl_remove(aux->subdirs_by_id, dir, cmp_ids);
		free(dir->subdirs_by_id);

		free(dir->rel_path);
		free(dir);
		dir = aux;
	}
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
	fs->root = NULL;
	return fs;
}

int fs_set(struct FS* fs, char* path, char* value) {
	struct File* f = new_file(path, value);
	struct Directory* dir;

	if (fs->root == NULL)
		fs->root = new_directory("/");

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

int fs_remove(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return 1; /* Not found */
	else {
		struct Directory* aux = dir->p;
		aux->subdirs_by_path = avl_remove(aux->subdirs_by_path, dir, cmp_ids);
		aux->subdirs_by_id = avl_remove(aux->subdirs_by_id, dir, cmp_ids);
		remove_directory(dir);
		prune_branch(aux);
	}

	return 0;
}

int fs_print(struct FS* fs) {
	print_all(fs->root);
	return 0;
}
