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

#include "tables/character.h"
#include "tables/description.h"
#include "tables/dialog.h"
#include "tables/inventory.h"
#include "tables/items.h"
#include "tables/location.h"
#include <stdio.h>

#define MAX_TABLES 6

enum Table {
    CHARACTERS = 0,
    DESCRIPTION,
    DIALOG,
    INVENTORY,
    ITEMS,
    LOCATIONS
};

struct Database {
    unsigned char initialized;
    FILE *file;
    void *tables[MAX_TABLES];
    unsigned int table_index_counters[MAX_TABLES];
};

struct Database *Database_create();
void Database_open(struct Database *db, const char *path);
void Database_close(struct Database *db);
void Database_flush(struct Database *db);
void Database_destroy(struct Database *db);
void Database_printAll(struct Database *db);

// Table-level ops
void *Database_get(struct Database *db, enum Table table);
void Database_set(struct Database *db, enum Table table, void *data);
void Database_write(struct Database *db, enum Table table);
void Database_delete(struct Database *db, enum Table table, int id);
void Database_print(struct Database *db, enum Table table);

unsigned int Database_getNextIndex(struct Database *db, enum Table table);

// Record-level ops (setters return index of record in table)
struct CharacterRecord *Database_getCharacter(struct Database *db, int id);
struct CharacterRecord *Database_getCharacterByName(struct Database *db, char *name);
int Database_createCharacter(struct Database *db, struct CharacterRecord *stats);
int Database_updateCharacter(struct Database *db, struct CharacterRecord *stats, int id);
void Database_deleteCharacter(struct Database *db, int id);

struct DescriptionRecord *Database_getDescription(struct Database *db, int id);
struct DescriptionRecord *Database_getDescriptionByPrefix(struct Database *db, char *prefix);
struct DescriptionRecord *Database_getOrCreateDescription(struct Database *db, char *prefix);
int Database_createDescription(struct Database *db, struct DescriptionRecord *description);
int Database_updateDescription(struct Database *db, struct DescriptionRecord *description, int id);
void Database_deleteDescription(struct Database *db, int id);

struct DialogRecord *Database_getDialog(struct Database *db, int id);
int Database_createDialog(struct Database *db, struct DialogRecord *dialog);
int Database_updateDialog(struct Database *db, struct DialogRecord *dialog, int id);
void Database_deleteDialog(struct Database *db, int id);

struct ItemRecord *Database_getItem(struct Database *db, int id);
struct ItemRecord *Database_getItemByName(struct Database *db, char *name);
struct ItemRecord *Database_getOrCreateItem(struct Database *db, char *name);
int Database_createItem(struct Database *db, struct ItemRecord *item);
int Database_updateItem(struct Database *db, struct ItemRecord *item, int id);
void Database_deleteItem(struct Database *db, int id);

struct InventoryRecord *Database_getInventory(struct Database *db, int id);
struct InventoryRecord *Database_getInventoryByOwner(struct Database *db, char *owner);
int Database_createInventory(struct Database *db, char *owner);
int Database_updateInventory(struct Database *db, struct InventoryRecord *record, int id);
void Database_deleteInventory(struct Database *db, int id);

struct ItemRecord **Database_getItemsInInventory(struct Database *db, char *owner);

struct LocationRecord *Database_getLocation(struct Database *db, int id);
struct LocationRecord *Database_getLocationByName(struct Database *db, char *name);
int Database_createLocation(struct Database *db, struct LocationRecord *location);
int Database_updateLocation(struct Database *db, struct LocationRecord *location, int id);
void Database_deleteLocation(struct Database *db, int id);
