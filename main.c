#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fs.h"

#define BUFF_SZ 65536
#define KEEP_GOING 0
#define STOP -1
#define EXIT_OK 0

#define HELP_HELP "help: Imprime os comandos disponíveis.\n"
#define HELP_QUIT "quit: Termina o programa.\n"
#define HELP_SET "set: Adiciona ou modifica o valor a armazenar.\n"
#define HELP_PRINT "print: Imprime todos os caminhos e valores.\n"
#define HELP_FIND "find: Imprime o valor armazenado.\n"
#define HELP_LIST "list: Lista todos os componentes imediatos de um sub-caminho.\n"
#define HELP_SEARCH "search: Procura o caminho dado um valor.\n"
#define HELP_DELETE "delete: Apaga um caminho e todos os subcaminhos."

#define ERR_MSG_NOT_FOUND "not found"
#define ERR_MSG_NO_DATA "no data"

int help() {
	puts(
		HELP_HELP
		HELP_QUIT
		HELP_SET
		HELP_PRINT
		HELP_FIND
		HELP_LIST
		HELP_SEARCH
		HELP_DELETE
	);
	return 0;
}

int set(struct FS* fs_store) {
	char path[BUFF_SZ], data[BUFF_SZ];
	scanf("%s%*[ ]%[^\n]", path, data);
	return fs_set(fs_store, path, data);
}

int print(struct FS* fs_store) {
	return fs_print(fs_store);
}

int find(struct FS* fs_store) {
	char path[BUFF_SZ];
	scanf("%s", path);
	return fs_find(fs_store, path);
}

int list(struct FS* fs_store) {
	char path[BUFF_SZ] = FS_ROOT;
	scanf("%*[ ]%s", path);
	return fs_list(fs_store, path);
}

int delete(struct FS* fs_store) {
	char path[BUFF_SZ] = FS_ROOT;
	scanf("%*[ ]%s", path);
	return fs_remove(fs_store, path);
}

int search(struct FS* fs_store) {
	char data[BUFF_SZ];
	scanf("%*[ ]%[^\n]", data);
	return fs_search(fs_store, data);
}

int quit(struct FS* fs_store) {
	fs_remove(fs_store, FS_ROOT);
	free(fs_store);
	return STOP;
}

int select(struct FS* fs_store) {
	char cmd[BUFF_SZ];
	scanf("%s", cmd);

	if (strcmp(cmd, "help") == 0)
		return help();
	else if (strcmp(cmd, "set") == 0)
		return set(fs_store);
	else if (strcmp(cmd, "print") == 0)
		return print(fs_store);
	else if (strcmp(cmd, "find") == 0)
		return find(fs_store);
	else if (strcmp(cmd, "list") == 0)
		return list(fs_store);
	else if (strcmp(cmd, "delete") == 0)
		return delete(fs_store);
	else if (strcmp(cmd, "search") == 0)
		return search(fs_store);
	else if (strcmp(cmd, "quit") == 0)
		return quit(fs_store);
	else
		return OK;
}

int main() {
	int status = KEEP_GOING;
	struct FS* fs_store = fs_init();

	while (status == KEEP_GOING) {
		switch (select(fs_store)) {
				case OK:
					break;
				case ERR_NOT_FOUND:
					puts(ERR_MSG_NOT_FOUND);
					break;
				case ERR_NO_DATA:
					puts(ERR_MSG_NO_DATA);
					break;
				case ERR_NO_MEMORY:
					quit(fs_store);
					status = STOP;
					break;
				case STOP:
					status = STOP;
					break;
			}
	}

	return EXIT_OK;
}

