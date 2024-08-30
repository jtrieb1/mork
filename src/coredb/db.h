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

#pragma once

#include "tables/character_stats.h"
#include "tables/description.h"
#include "tables/dialog.h"
#include "tables/inventory.h"
#include "tables/items.h"
#include <stdio.h>

#define MAX_TABLES 5

enum Table {
    CHARACTER_STATS = 0,
    DESCRIPTION,
    DIALOG,
    INVENTORY,
    ITEMS
};

struct Database {
    unsigned char initialized;
    FILE *file;
    void *tables[MAX_TABLES];
};

struct Database *Database_create();
void Database_open(struct Database *db, const char *path);
void Database_close(struct Database *db);
void Database_flush(struct Database *db);
void Database_destroy(struct Database *db);
void Database_print_all(struct Database *db);

// Table-level ops
void *Database_get(struct Database *db, enum Table table);
void Database_set(struct Database *db, enum Table table, void *data);
void Database_write(struct Database *db, enum Table table);
void Database_delete(struct Database *db, enum Table table, int id);
void Database_print(struct Database *db, enum Table table);

// Record-level ops
struct CharacterStatsRecord *Database_get_character_stats(struct Database *db, int id);
void Database_set_character_stats(struct Database *db, struct CharacterStatsRecord *stats);

struct DescriptionRecord *Database_get_description(struct Database *db, int id);
void Database_set_description(struct Database *db, struct DescriptionRecord *description);

struct DialogRecord *Database_get_dialog(struct Database *db, int id);
void Database_set_dialog(struct Database *db, struct DialogRecord *dialog);

struct ItemRecord *Database_get_item(struct Database *db, int id);
void Database_set_item(struct Database *db, struct ItemRecord *item);

struct InventoryRecord *Database_get_inventory(struct Database *db, int id);
void Database_set_inventory(struct Database *db, struct InventoryRecord *inventory);
