/* struct FS; */
struct FS {struct Directory* root;};

struct FS* fs_init();
int fs_set(struct FS* fs, char* path, char* value);
int fs_remove(struct FS* fs, char* path);
int fs_find(struct FS* fs, char* path);
int fs_list(struct FS* fs, char* path);
int fs_print(struct FS* fs);
int fs_destroy(struct FS* fs);


char* strdup(char* str);
