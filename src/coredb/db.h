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

#include "../utils/error.h"

#include "tables/character.h"
#include "tables/description.h"
#include "tables/dialog.h"
#include "tables/games.h"
#include "tables/inventory.h"
#include "tables/items.h"
#include "tables/location.h"
#include <stdio.h>

// In Mork, we have Models and Records that back them.
// A model is a struct that represents a game object, like an Inventory or an Item.
// A record is a struct that represents a row in a table in the database.
// A model may refer to multiple records, and a record may be referred to by multiple models.

#define MAX_TABLES 7

enum Table {
    CHARACTERS = 0,
    DESCRIPTION,
    DIALOG,
    GAMES,
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
enum MorkResult Database_createFile(struct Database *db, const char *path);
enum MorkResult Database_open(struct Database *db, const char *path);
enum MorkResult Database_close(struct Database *db);
enum MorkResult Database_flush(struct Database *db);
enum MorkResult Database_destroy(struct Database *db);

// Table-level ops
void *Database_get(struct Database *db, enum Table table);
enum MorkResult Database_set(struct Database *db, enum Table table, void *data);
enum MorkResult Database_write(struct Database *db, enum Table table);
enum MorkResult Database_delete(struct Database *db, enum Table table);
enum MorkResult Database_print(struct Database *db, enum Table table);

unsigned int Database_getNextIndex(struct Database *db, enum Table table);

// Record-level ops (setters return index of record in table)
struct CharacterRecord *Database_getCharacter(struct Database *db, int id);
struct CharacterRecord *Database_getCharacterByName(struct Database *db, char *name);
enum MorkResult Database_createCharacter(struct Database *db, struct CharacterRecord *stats);
enum MorkResult Database_updateCharacter(struct Database *db, struct CharacterRecord *stats);
enum MorkResult Database_deleteCharacter(struct Database *db, int id);

struct DescriptionRecord *Database_getDescription(struct Database *db, int id);
struct DescriptionRecord *Database_getDescriptionByPrefix(struct Database *db, char *prefix);
struct DescriptionRecord *Database_getOrCreateDescription(struct Database *db, char *prefix);
enum MorkResult Database_createDescription(struct Database *db, struct DescriptionRecord *description);
enum MorkResult Database_updateDescription(struct Database *db, struct DescriptionRecord *description);
enum MorkResult Database_deleteDescription(struct Database *db, int id);

struct DialogRecord *Database_getDialog(struct Database *db, int id);
enum MorkResult Database_createDialog(struct Database *db, struct DialogRecord *dialog);
enum MorkResult Database_updateDialog(struct Database *db, struct DialogRecord *dialog);
enum MorkResult Database_deleteDialog(struct Database *db, int id);

struct ItemRecord *Database_getItem(struct Database *db, int id);
struct ItemRecord *Database_getItemByName(struct Database *db, char *name);
enum MorkResult Database_createItem(struct Database *db, struct ItemRecord *item);
enum MorkResult Database_updateItem(struct Database *db, struct ItemRecord *item);
enum MorkResult Database_deleteItem(struct Database *db, int id);

struct InventoryRecord *Database_getInventory(struct Database *db, int id);
struct InventoryRecord *Database_getInventoryByOwner(struct Database *db, char *owner);
enum MorkResult Database_createInventory(struct Database *db, char *owner);
enum MorkResult Database_updateInventory(struct Database *db, struct InventoryRecord *record);
enum MorkResult Database_deleteInventory(struct Database *db, int id);

struct ItemRecord **Database_getItemsInInventory(struct Database *db, char *owner);

struct LocationRecord *Database_getLocation(struct Database *db, int id);
struct LocationRecord *Database_getLocationByName(struct Database *db, char *name);
enum MorkResult Database_createLocation(struct Database *db, struct LocationRecord *location);
enum MorkResult Database_updateLocation(struct Database *db, struct LocationRecord *location);
enum MorkResult Database_deleteLocation(struct Database *db, int id);

struct GameRecord *Database_getGame(struct Database *db, int id);
enum MorkResult Database_createGame(struct Database *db, struct GameRecord *game);
enum MorkResult Database_updateGame(struct Database *db, struct GameRecord *game);
enum MorkResult Database_deleteGame(struct Database *db, int id);