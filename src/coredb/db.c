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

#include "db.h"

#include <assert.h>
#include <lcthw/dbg.h>

void Database_init(struct Database *db)
{
    if (db->initialized == 1) {
        return; // Don't re-initialize
    }
    // Initialize all tables
    db->tables[CHARACTERS] = CharacterTable_create();
    CharacterTable_init(db->tables[CHARACTERS]);
    db->tables[DIALOG] = DialogTable_create();
    DialogTable_init(db->tables[DIALOG]);
    db->tables[GAMES] = GameTable_create();
    GameTable_init(db->tables[GAMES]);
    db->tables[ITEMS] = ItemTable_create();
    ItemTable_init(db->tables[ITEMS]);
    db->tables[DESCRIPTION] = DescriptionTable_create();
    DescriptionTable_init(db->tables[DESCRIPTION]);
    db->tables[INVENTORY] = InventoryTable_create();
    InventoryTable_init(db->tables[INVENTORY]);
    db->tables[LOCATIONS] = LocationTable_create();
    db->initialized = 1;

}

long table_offset(enum Table table)
{
    assert(table >= 0 && table < MAX_TABLES);

    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4065)
    #endif
    long offset = 0;
    switch (table) {
        case LOCATIONS:
            offset += sizeof(struct ItemTable);
        case ITEMS:
            offset += sizeof(struct InventoryTable);
        case INVENTORY:
            offset += sizeof(struct GameTable);
        case GAMES:
            offset += sizeof(struct DialogTable);
        case DIALOG:
            offset += sizeof(struct DescriptionTable);
        case DESCRIPTION:
            offset += sizeof(struct CharacterTable);
        case CHARACTERS:
            break;
        default:
            sentinel("Unknown table type");
    }

    return offset;

error:
    return -1;
}

size_t table_size(enum Table table)
{
    assert(table >= 0 && table < MAX_TABLES);

    switch (table) {
        case CHARACTERS:
            return sizeof(struct CharacterTable);
        case DESCRIPTION:
            return sizeof(struct DescriptionTable);
        case DIALOG:
            return sizeof(struct DialogTable);
        case GAMES:
            return sizeof(struct GameTable);
        case INVENTORY:
            return sizeof(struct InventoryTable);
        case ITEMS:
            return sizeof(struct ItemTable);
        case LOCATIONS:
            return sizeof(struct LocationTable);
        default:
            sentinel("Unknown table type");
    }

error:
    return 0;
}

enum MorkResult Database_createFile(struct Database *db, const char *path)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (path == NULL) { return MORK_ERROR_DB_INVALID_PATH; }

    if (db->file) {
        fclose(db->file);
    }

    db->file = fopen(path, "w+");
    check(db->file, "Failed to create file: %s", path);

    Database_init(db);

    // Write out the tables to disk
    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        Database_write(db, tbl);
    }

    // Close file to flush to disk
    fclose(db->file);

    return MORK_OK;

error:
    return MORK_ERROR_DB;
}

enum MorkResult Database_open(struct Database *db, const char *path)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (path == NULL) { return MORK_ERROR_DB_INVALID_PATH; }

    db->file = fopen(path, "rw+");
    check(db->file, "Failed to open file: %s", path);

    Database_init(db);

    // If the file is empty, don't bother reading anything
    if (fseek(db->file, 0, SEEK_END) == 0) {
        long size = ftell(db->file);
        if (size == 0) {
            return MORK_OK;
        }
    }

    // Load the database from disk
    // The database file is just each table written out in order
    // so we can just read them in order by offsets

    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        fseek(db->file, table_offset(tbl), SEEK_SET);
        check(fread(db->tables[tbl], table_size(tbl), 1, db->file), "Failed to read table %d", tbl);
    }
    return MORK_OK;

error:
    return MORK_ERROR_DB;
}

enum MorkResult Database_close(struct Database *db)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    
    if (db->initialized != 1) {
        Database_init(db);
    }

    if (db->file) {
        // Make sure we write out to the file before closing it
        enum MorkResult res = Database_flush(db);
        if (res != MORK_OK) { return res; }
        fclose(db->file);
        db->file = NULL;
    }

    return MORK_OK;
}

enum MorkResult Database_flush(struct Database *db)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }

    // Write the entire database to disk
    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        if (db->tables[tbl]) {
            Database_write(db, tbl);
        }
    }

    if (db->file) {
        // Just to be extra sure
        fflush(db->file);
    }
    return MORK_OK;
}

struct Database *Database_create()
{
    struct Database *db = calloc(1, sizeof(struct Database));
    check_mem(db);

    db->file = NULL;
    db->initialized = 0;
    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        db->table_index_counters[tbl] = 1;
    }

    for (int i = 0; i < MAX_TABLES; i++) {
        db->tables[i] = NULL;
    }

    Database_init(db);

    return db;

error:
    return NULL;
}

enum MorkResult Database_destroy(struct Database *db)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }

    // Make sure we close before we destroy
    enum MorkResult close_result = Database_close(db);
    if (close_result != MORK_OK) { return close_result; }

    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        enum MorkResult res = MORK_OK;

        switch (tbl) {
            case CHARACTERS:
                if (db->tables[tbl] ==  NULL) break;
                res = CharacterTable_destroy((struct CharacterTable *)db->tables[tbl]);
                break;
            case DESCRIPTION:
                if (db->tables[tbl] == NULL) break;
                res = DescriptionTable_destroy((struct DescriptionTable *)db->tables[tbl]);
                break;
            case DIALOG:
                if (db->tables[tbl] == NULL) break;
                res = DialogTable_destroy((struct DialogTable *)db->tables[tbl]);
                break;
            case GAMES:
                if (db->tables[tbl] == NULL) break;
                res = GameTable_destroy((struct GameTable *)db->tables[tbl]);
                break;
            case INVENTORY:
                if (db->tables[tbl] == NULL) break;
                res = InventoryTable_destroy((struct InventoryTable *)db->tables[tbl]);
                break;
            case ITEMS:
                if (db->tables[tbl] == NULL) break;
                res = ItemTable_destroy((struct ItemTable *)db->tables[tbl]);
                break;
            case LOCATIONS:
                if (db->tables[tbl] == NULL) break;
                res = LocationTable_destroy((struct LocationTable *)db->tables[tbl]);
                break;
            default:
                return MORK_ERROR_DB;
        }

        if (res != MORK_OK) { return res; }
    }

    db->initialized = 0;

    free(db);
    return MORK_OK;
}

void *Database_get(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    return db->tables[table];

error:
    return NULL;
}

enum MorkResult Database_set(struct Database *db, enum Table table, void *data)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (table < 0 || table >= MAX_TABLES) { return MORK_ERROR_DB_INVALID_DATA; }

    db->tables[table] = data;
    return MORK_OK;
}

enum MorkResult Database_write(struct Database *db, enum Table table)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (table < 0 || table >= MAX_TABLES) { return MORK_ERROR_DB_INVALID_DATA; }
    if (db->file == NULL) { return MORK_ERROR_DB_FILE_NULL; }

    // We know the sizes of the individual tables, so we can write them directly via offset writes
    int seekres  = fseek(db->file, table_offset(table), SEEK_SET);
    if (seekres != 0) { return MORK_ERROR_DB_FILE_SEEK; }

    size_t writeres = fwrite(db->tables[table], table_size(table), 1, db->file);
    if (writeres != table_size(table)) { return MORK_ERROR_DB_FILE_WRITE; }

    int flushres = fflush(db->file);
    if (flushres != 0) { return MORK_ERROR_DB_FILE_FLUSH; }

    // Return seek back to the beginning of the file
    fseek(db->file, 0, SEEK_SET);
    return MORK_OK;
}

enum MorkResult Database_delete(struct Database *db, enum Table table)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    switch (table) {
        case CHARACTERS:
            return CharacterTable_destroy((struct CharacterTable *)db->tables[CHARACTERS]);
        case DESCRIPTION:
            return DescriptionTable_destroy((struct DescriptionTable *)db->tables[DESCRIPTION]);
        case DIALOG:
            return DialogTable_destroy((struct DialogTable *)db->tables[DIALOG]);
        case GAMES:
            return GameTable_destroy((struct GameTable *)db->tables[GAMES]);
        case INVENTORY:
            return InventoryTable_destroy((struct InventoryTable *)db->tables[INVENTORY]);
        case ITEMS:
            return ItemTable_destroy((struct ItemTable *)db->tables[ITEMS]);
        case LOCATIONS:
            return LocationTable_destroy((struct LocationTable *)db->tables[LOCATIONS]);
        default:
            return MORK_ERROR_DB_INVALID_DATA;
    }

error:
    return MORK_ERROR_DB;
}

enum MorkResult Database_print(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    switch (table) {
        case CHARACTERS:
            return CharacterTable_print((struct CharacterTable *)db->tables[CHARACTERS]);
        case DESCRIPTION:
            return DescriptionTable_print((struct DescriptionTable *)db->tables[DESCRIPTION]);
        case DIALOG:
            return DialogTable_print((struct DialogTable *)db->tables[DIALOG]);
        case GAMES:
            return GameTable_print((struct GameTable *)db->tables[GAMES]);
        case INVENTORY:
            return InventoryTable_print((struct InventoryTable *)db->tables[INVENTORY]);
        case ITEMS:
            return ItemTable_list((struct ItemTable *)db->tables[ITEMS]);
        case LOCATIONS:
            return LocationTable_print((struct LocationTable *)db->tables[LOCATIONS]);
        default:
            return MORK_ERROR_DB_INVALID_DATA;
    }

error:
    return MORK_ERROR_DB;
}

unsigned int Database_getNextIndex(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);
    return db->table_index_counters[table]++;

error:
    return 0;
}

struct CharacterRecord *Database_getCharacter(struct Database *db, int id)
{
    check(db != NULL, "Database is NULL");

    struct CharacterTable *table = db->tables[CHARACTERS];
    check(table != NULL, "Character table is NULL");

    return CharacterTable_get(table, id);

error:
    return NULL;
}

struct CharacterRecord *Database_getCharacterByName(struct Database *db, char *name)
{
    check(db != NULL, "Database is NULL");

    struct CharacterTable *table = db->tables[CHARACTERS];
    check(table != NULL, "Character table is NULL");

    return CharacterTable_getByName(table, name);

error:
    return NULL;
}

enum MorkResult Database_createCharacter(struct Database *db, struct CharacterRecord *stats)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (stats == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct CharacterTable *table = db->tables[CHARACTERS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return CharacterTable_newRow(table, stats);
}

enum MorkResult Database_updateCharacter(struct Database *db, struct CharacterRecord *stats)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (stats == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct CharacterTable *table = db->tables[CHARACTERS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return CharacterTable_update(table, stats);
}

enum MorkResult Database_deleteCharacter(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct CharacterTable *table = db->tables[CHARACTERS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return CharacterTable_delete(table, id);
}

struct DialogRecord *Database_getDialog(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct DialogTable *table = db->tables[DIALOG];
    check(table != NULL, "Dialog table is not initialized.");

    return DialogTable_get(table, id);

error:
    return NULL;
}

enum MorkResult Database_createDialog(struct Database *db, struct DialogRecord *dialog)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (dialog == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct DialogTable *table = db->tables[DIALOG];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DialogTable_newRow(table, dialog);
}

enum MorkResult Database_updateDialog(struct Database *db, struct DialogRecord *dialog)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (dialog == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct DialogTable *table = db->tables[DIALOG];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DialogTable_update(table, dialog);
}

enum MorkResult Database_deleteDialog(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct DialogTable *table = db->tables[DIALOG];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DialogTable_delete(table, id);
}

struct ItemRecord *Database_getItem(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    return ItemTable_get(table, id);

error:
    return NULL;
}

struct ItemRecord *Database_getItemByName(struct Database *db, char *name)
{
    check(db != NULL, "Expected a non-null database.");
    check(name != NULL && strcmp(name, "") != 0, "Expected a valid name");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    return ItemTable_getByName(table, name);

error:
    return NULL;
}

enum MorkResult Database_createItem(struct Database *db, struct ItemRecord *item)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (item == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct ItemTable *table = db->tables[ITEMS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return ItemTable_newRow(table, item);
}

enum MorkResult Database_updateItem(struct Database *db, struct ItemRecord *item)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (item == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    
    struct ItemTable *table = db->tables[ITEMS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return ItemTable_update(table, item);
}

struct DescriptionRecord *Database_getDescription(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    return DescriptionTable_get(table, id);

error:
    return NULL;
}

struct DescriptionRecord *Database_getDescriptionByPrefix(struct Database *db, char *prefix)
{
    check(db != NULL, "Expected a non-null database.");
    check(prefix != NULL, "Expected a valid prefix");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    return DescriptionTable_get_by_prefix(table, prefix);

error:
    return NULL;
}

struct DescriptionRecord *Database_getOrCreateDescription(struct Database *db, char *prefix)
{
    check(db != NULL, "Expected a non-null database.");
    check(prefix != NULL, "Expected a valid prefix");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    struct DescriptionRecord *desc = DescriptionTable_get_by_prefix(table, prefix);
    if (desc == NULL) {
        desc = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), prefix, 0);
        enum MorkResult res = Database_createDescription(db, desc);
        check(res == MORK_OK, "Failed to create description record.");
        desc = Database_getDescriptionByPrefix(db, prefix);
    }

    return desc;

error:
    return NULL;
}

enum MorkResult Database_createDescription(struct Database *db, struct DescriptionRecord *desc)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (desc == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DescriptionTable_insert(table, desc);
}

enum MorkResult Database_updateDescription(struct Database *db, struct DescriptionRecord *desc)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (desc == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DescriptionTable_update(table, desc);
}

enum MorkResult Database_deleteDescription(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return DescriptionTable_delete(table, id);
}

struct InventoryRecord *Database_getInventory(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    return InventoryTable_get(table, id);

error:
    return NULL;
}

struct InventoryRecord *Database_getInventoryByOwner(struct Database *db, char *owner)
{
    check(db != NULL, "Expected a non-null database.");
    check(owner != NULL && strcmp(owner, "") != 0, "Expected a valid owner.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    struct CharacterRecord *owner_record = Database_getCharacterByName(db, owner);
    check(owner_record != NULL, "Character stats record not found.");

    return InventoryTable_getByOwner(table, owner_record->id);

error:
    return NULL;
}

enum MorkResult Database_createInventory(struct Database *db, char *owner)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (owner == NULL || strcmp(owner, "") == 0) { return MORK_ERROR_DB_INVALID_DATA; }

    struct InventoryTable *table = db->tables[INVENTORY];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    struct CharacterRecord *owner_record = Database_getCharacterByName(db, owner);
    if (owner_record == NULL) { return MORK_ERROR_DB_NOT_FOUND; }

    return InventoryTable_add(table, owner_record->id, Database_getNextIndex(db, INVENTORY));
}

enum MorkResult Database_updateInventory(struct Database *db, struct InventoryRecord *record)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct InventoryTable *table = db->tables[INVENTORY];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return InventoryTable_update(table, record);
}

enum MorkResult Database_deleteInventory(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct InventoryTable *table = db->tables[INVENTORY];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return InventoryTable_remove(table, id);
}

struct ItemRecord **Database_getItemsInInventory(struct Database *db, char *owner)
{
    check(db != NULL, "Expected a non-null database.");

    struct InventoryRecord *inventory = Database_getInventoryByOwner(db, owner);
    check(inventory != NULL, "Inventory record not found.");

    struct ItemRecord **items = calloc(InventoryRecord_getItemCount(inventory), sizeof(struct ItemRecord *));
    check_mem(items);

    for (int i = 0; i < InventoryRecord_getItemCount(inventory); i++)
    {
        if (inventory->item_ids[i] == 0) { continue; }
        items[i] = Database_getItem(db, inventory->item_ids[i]);
    }

    return items;

error:
    return NULL;
}

enum MorkResult Database_deleteItem(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct ItemTable *table = db->tables[ITEMS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return ItemTable_delete(table, id);
}

struct LocationRecord *Database_getLocation(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct LocationTable *table = db->tables[LOCATIONS];
    check(table != NULL, "Location table is not initialized.");

    return LocationTable_get(table, id);

error:
    return NULL;
}

struct LocationRecord *Database_getLocationByName(struct Database *db, char *name)
{
    check(db != NULL, "Expected a non-null database.");
    check(name != NULL && strcmp(name, "") != 0, "Expected a valid name.");

    struct LocationTable *table = db->tables[LOCATIONS];
    check(table != NULL, "Location table is not initialized.");

    return LocationTable_getByName(table, name);

error:
    return NULL;
}

enum MorkResult Database_createLocation(struct Database *db, struct LocationRecord *location)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (location == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct LocationTable *table = db->tables[LOCATIONS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return LocationTable_add(table, location);
}

enum MorkResult Database_updateLocation(struct Database *db, struct LocationRecord *location)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (location == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct LocationTable *table = db->tables[LOCATIONS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return LocationTable_update(table, location);
}

enum MorkResult Database_deleteLocation(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct LocationTable *table = db->tables[LOCATIONS];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return LocationTable_remove(table, id);
}

struct GameRecord *Database_getGame(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");
    check(id > 0, "Expected a valid ID.");

    struct GameTable *table = db->tables[GAMES];
    check(table != NULL, "Game table is not initialized.");

    return GameTable_get(table, id);

error:
    return NULL;
}

enum MorkResult Database_createGame(struct Database *db, struct GameRecord *game)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (game == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct GameTable *table = db->tables[GAMES];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return GameTable_insert(table, game);
}

enum MorkResult Database_updateGame(struct Database *db, struct GameRecord *game)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (game == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    struct GameTable *table = db->tables[GAMES];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return GameTable_update(table, game);
}

enum MorkResult Database_deleteGame(struct Database *db, int id)
{
    if (db == NULL) { return MORK_ERROR_DB_NULL; }
    if (id <= 0) { return MORK_ERROR_DB_INVALID_ID; }

    struct GameTable *table = db->tables[GAMES];
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    return GameTable_delete(table, id);
}
