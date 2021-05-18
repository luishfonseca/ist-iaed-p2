struct FS;

struct FS* fs_init();
int fs_set(struct FS* fs, char* path, char* value);
int fs_find(struct FS* fs, char* path);
int fs_list(struct FS* fs, char* path);
int fs_print(struct FS* fs);
char* strdup(char* str);
