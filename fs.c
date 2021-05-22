#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "avl.h"
#include "hashtable.h"
#include "fs.h"

struct Directory {
	int id;
	int depth;
	char* path;
	char* value;
	struct Directory* p;
	struct AVL* subdirs_by_id;
	struct AVL* subdirs_by_path;
};

struct FS {
	struct Directory* root;
	struct HashTable* lookup;
};

char* strdup(char* str) {
	char* new_str = malloc(strlen(str) + 1);
	if (new_str == NULL)
		return NULL;
	return strcpy(new_str, str);
}

struct Directory* new_directory(char* rel_path, int depth) {
	static int id = 0;
	struct Directory* dir = malloc(sizeof(struct Directory));
	if (dir == NULL)
		return NULL;
	dir->id = id++;
	dir->depth = depth;
	dir->path = strdup(rel_path);
	if (dir->path == NULL)
		return NULL;
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

char* dir_value(void* dir) {
	return ((struct Directory*)dir)->value;
}

int more_recent(void* dir1, void* dir2) {
	struct Directory* new_dir = dir1;
	struct Directory* old_dir = dir2;

	/* If old was NULL new must be more recent */
	if (old_dir == NULL)
		return 1;

	/* Backtrack the deepest until both have same depth */
	while (new_dir->depth > old_dir->depth)
		new_dir = new_dir->p;
	while (old_dir->depth > new_dir->depth)
		old_dir = old_dir->p;

	/* Backtrack both until they have the same parent */
	while (new_dir->p != old_dir->p) {
		new_dir = new_dir->p;
		old_dir = old_dir->p;
	}

	/* Compare order of creation, return true if new_dir is more recent */
	return new_dir->id < old_dir->id;
}

void print_dir_relative_path(void* d, void* extra) {
	struct Directory* dir = d;

	(void)extra;

	printf("%s\n", dir->path);
}

void print_dir_full_path(struct Directory* dir) {
	if (dir->p != NULL && strcmp(dir->p->path, FS_ROOT) != 0)
		print_dir_full_path(dir->p);

	printf("/%s", dir->path);
}

struct Directory* create_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path = strtok(path, PATH_DELIMITER);

	if (rel_path == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL) {
		sub = new_directory(rel_path, dir->depth + 1);
		if (sub == NULL)
			return NULL;
		sub->p = dir;
		dir->subdirs_by_id = avl_insert(dir->subdirs_by_id, sub, cmp_ids);
		dir->subdirs_by_path = avl_insert(dir->subdirs_by_path, sub, cmp_paths);
		if (dir->subdirs_by_id == NULL || dir->subdirs_by_path == NULL)
			return NULL;
	}

	return create_directory(sub, NULL);
}

struct Directory* find_directory(struct Directory* dir, char* path) {
	struct Directory* sub;
	char* rel_path;

	if (dir == NULL)
		return NULL;

	if ((rel_path = strtok(path, PATH_DELIMITER)) == NULL)
		return dir;

	sub = avl_find(dir->subdirs_by_path, rel_path, search_path);
	if (sub == NULL)
		return NULL;
	else
		return find_directory(sub, NULL);
}

void remove_directory(void* d, void* extra) {
	struct Directory* dir = d;
	struct FS* fs = extra;

	if (dir != NULL) {
		if (dir->value != NULL) {
			fs->lookup = ht_remove(fs->lookup, dir, dir_value, cmp_ids);
			free(dir->value);
		}

		avl_traverse(dir->subdirs_by_id, remove_directory, extra);

		avl_destroy(dir->subdirs_by_path);
		avl_destroy(dir->subdirs_by_id);
		free(dir->path);
		free(dir);
	}
}

void print_all(void* d, void* extra) {
	struct Directory* dir = d;

	/* This recursion doesn't require extra arguments */
	(void)(extra);

	if (dir != NULL) {
		if (dir->value != NULL) {
			print_dir_full_path(dir);
			printf(" %s\n", dir->value);
		}

		avl_traverse(dir->subdirs_by_id, print_all, NULL);
	}
}

struct FS* fs_init() {
	struct FS* fs = malloc(sizeof(struct FS));
	if (fs == NULL)
		return NULL;
	fs->root = NULL;
	fs->lookup = NULL;
	return fs;
}

int fs_set(struct FS* fs, char* path, char* value) {
	struct Directory* dir;

	if (fs->root == NULL) {
		fs->root = new_directory(FS_ROOT, 0);
		if (fs->root == NULL)
			return ERR_NO_MEMORY;
	}

	dir = create_directory(fs->root, path);
	if (dir == NULL)
		return ERR_NO_MEMORY;

	if (dir->value != NULL) {
		fs->lookup = ht_remove(fs->lookup, dir, dir_value, cmp_ids);
		free(dir->value);
	}
	dir->value = strdup(value);
	if (dir->value == NULL)
		return ERR_NO_MEMORY;

	fs->lookup = ht_insert(fs->lookup, dir, dir_value);
	if (fs->lookup == NULL)
		return ERR_NO_MEMORY;

	return OK;
}

int fs_find(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return ERR_NOT_FOUND; /* Not found */
	else if (dir->value == NULL)
		return ERR_NO_DATA; /* No data */

	printf("%s\n", dir->value);

	return OK;
}

int fs_list(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return ERR_NOT_FOUND; /* Not found */

	avl_traverse(dir->subdirs_by_path, print_dir_relative_path, NULL);

	return OK;
}

int fs_search(struct FS* fs, char* v) {
	struct Directory* dir = ht_search(fs->lookup, v, dir_value, more_recent);

	if (dir == NULL)
		return ERR_NOT_FOUND; /* Not found */

	print_dir_full_path(dir);
	printf("\n");


	return OK;
}

int fs_remove(struct FS* fs, char* path) {
	struct Directory* dir = find_directory(fs->root, path);

	if (dir == NULL)
		return ERR_NOT_FOUND; /* Not found */

	if (dir->p != NULL) {
		dir->p->subdirs_by_path =
			avl_remove(dir->p->subdirs_by_path, dir, cmp_paths);
		dir->p->subdirs_by_id =
			avl_remove(dir->p->subdirs_by_id, dir, cmp_ids);
	}

	remove_directory(dir, fs);

	if (dir == fs->root) {
		fs->root = NULL;
		ht_destroy(fs->lookup);
		fs->lookup = NULL;
	}

	return OK;
}

int fs_print(struct FS* fs) {
	print_all(fs->root, NULL);
	return OK;
}
