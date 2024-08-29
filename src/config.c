/*
Mork: A Zorklike text adventure game influenced by classic late 70s television.
Copyright (C) 2024 Jacob Triebwasser

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include <lcthw/dbg.h>

struct MorkConfig *MorkConfig_default() {
    struct MorkConfig *config = (struct MorkConfig *)malloc(sizeof(struct MorkConfig));
    check_mem(config);
    config->character_db_path = strdup("mork.db");
    return config;

error:
    return NULL;
}

void MorkConfig_free(struct MorkConfig *config) {
    free(config);
}

void MorkConfig_print(struct MorkConfig *config) {
    printf("Character DB Path: %s\n", config->character_db_path);
}

void MorkConfig_set_base_path(struct MorkConfig *config, const char *path) {
    unsigned int len = strlen(path) + strlen(config->character_db_path) + 2;
    char *new_path = (char *)malloc(len);
    check_mem(new_path);
    snprintf(new_path, len, "%s/%s", path, config->character_db_path);
    config->character_db_path = new_path;
    return;

error:
    return;
}

struct MorkConfig *MorkConfig_load(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return NULL;
    }

    struct MorkConfig *config = (struct MorkConfig *)malloc(sizeof(struct MorkConfig));
    if (config == NULL) {
        return NULL;
    }

    // Parse .ini file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            continue;
        }

        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");
        if (strcmp(key, "character_db_path") == 0) {
            // Normalize path
            if (value[strlen(value) - 1] == '\n') {
                value[strlen(value) - 1] = '\0';
            }
            config->character_db_path = strdup(value);
        }
    }

    fclose(file);
    return config;
}
