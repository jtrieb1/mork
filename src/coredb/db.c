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
    // Initialize all tables
    db->tables[CHARACTER_STATS] = CharacterTable_create();
    db->tables[DIALOG] = DialogTable_create();
    db->tables[ITEMS] = ItemTable_create();
    db->tables[DESCRIPTION] = DescriptionTable_create();
    db->tables[INVENTORY] = InventoryTable_create();
    db->initialized = 1;

}

long table_offset(enum Table table)
{
    assert(table >= 0 && table < MAX_TABLES);

    long offset = 0;
    for (enum Table tbl = 0; tbl < table; tbl++) {
        switch (tbl) {
            case CHARACTER_STATS:
                offset += sizeof(struct CharacterTable);
                break;
            case DESCRIPTION:
                offset += sizeof(struct DescriptionTable);
                break;
            case DIALOG:
                offset += sizeof(struct DialogTable);
                break;
            case INVENTORY:
                offset += sizeof(struct InventoryTable);
                break;
            case ITEMS:
                offset += sizeof(struct ItemTable);
                break;
            default:
                sentinel("Unknown table type");
        }
    }

    return offset;

error:
    return -1;
}

size_t table_size(enum Table table)
{
    assert(table >= 0 && table < MAX_TABLES);

    switch (table) {
        case CHARACTER_STATS:
            return sizeof(struct CharacterTable);
        case DESCRIPTION:
            return sizeof(struct DescriptionTable);
        case DIALOG:
            return sizeof(struct DialogTable);
        case INVENTORY:
            return sizeof(struct InventoryTable);
        case ITEMS:
            return sizeof(struct ItemTable);
        default:
            sentinel("Unknown table type");
    }

error:
    return 0;
}

void Database_open(struct Database *db, const char *path)
{
    assert(db != NULL);
    assert(path != NULL);

    db->file = fopen(path, "r+");
    check(db->file, "Failed to open file: %s", path);

    Database_init(db);

    // If the file is empty, don't bother reading anything
    if (fseek(db->file, 0, SEEK_END) == 0) {
        long size = ftell(db->file);
        if (size == 0) {
            return;
        }
    }

    // Load the database from disk
    // The database file is just each table written out in order
    // so we can just read them in order by offsets

    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        fseek(db->file, table_offset(tbl), SEEK_SET);
        check(fread(db->tables[tbl], table_size(tbl), 1, db->file), "Failed to read table %d", tbl);
    }

error:
    return;
}

void Database_close(struct Database *db)
{
    assert(db != NULL);
    if (db->initialized != 1) {
        Database_init(db);
    }

    if (db->file) {
        // Make sure we write out to the file before closing it
        Database_flush(db);
        fclose(db->file);
        db->file = NULL;
    }
}

void Database_flush(struct Database *db)
{
    assert(db != NULL);

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
}

struct Database *Database_create()
{
    struct Database *db = calloc(1, sizeof(struct Database));
    check_mem(db);

    db->file = NULL;
    db->initialized = 0;
    for (enum Table tbl = 0; tbl < MAX_TABLES; ++tbl) {
        db->table_index_counters[tbl] = 1;
    }

    for (int i = 0; i < MAX_TABLES; i++) {
        db->tables[i] = NULL;
    }

    return db;

error:
    return NULL;
}

void Database_destroy(struct Database *db)
{
    assert(db != NULL);

    // Make sure we close before we destroy
    Database_close(db);

    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        switch (tbl) {
            case CHARACTER_STATS:
                if (db->tables[tbl] ==  NULL) break;
                CharacterTable_destroy((struct CharacterTable *)db->tables[tbl]);
                break;
            case DESCRIPTION:
                if (db->tables[tbl] == NULL) break;
                DescriptionTable_destroy((struct DescriptionTable *)db->tables[tbl]);
                break;
            case DIALOG:
                if (db->tables[tbl] == NULL) break;
                DialogTable_destroy((struct DialogTable *)db->tables[tbl]);
                break;
            case INVENTORY:
                if (db->tables[tbl] == NULL) break;
                InventoryTable_destroy((struct InventoryTable *)db->tables[tbl]);
                break;
            case ITEMS:
                if (db->tables[tbl] == NULL) break;
                ItemTable_destroy((struct ItemTable *)db->tables[tbl]);
                break;
            default:
                sentinel("Unknown table: %d", tbl);
        }
    }

error:
    free(db);
}

void *Database_get(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    return db->tables[table];

error:
    return NULL;
}

void Database_set(struct Database *db, enum Table table, void *data)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    db->tables[table] = data;

error:
    return;
}

void Database_write(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);

    if (db->file) {
        // We know the sizes of the individual tables, so we can write them directly via offset writes
        fseek(db->file, table_offset(table), SEEK_SET);
        fwrite(db->tables[table], table_size(table), 1, db->file);
    }

error:
    return;
}

unsigned int Database_getNextIndex(struct Database *db, enum Table table)
{
    check(db != NULL, "Database is NULL");
    check(table >= 0 && table < MAX_TABLES, "Invalid table: %d", table);
    return db->table_index_counters[table]++;

error:
    return 0;
}

struct CharacterRecord *Database_getCharacterStats(struct Database *db, int id)
{
    check(db != NULL, "Database is NULL");

    struct CharacterTable *table = db->tables[CHARACTER_STATS];
    check(table != NULL, "Character stats table is NULL");

    return CharacterTable_get(table, id);

error:
    return NULL;
}

struct CharacterRecord *Database_getCharacterStatsByName(struct Database *db, char *name)
{
    check(db != NULL, "Database is NULL");

    struct CharacterTable *table = db->tables[CHARACTER_STATS];
    check(table != NULL, "Character stats table is NULL");

    return CharacterTable_getByName(table, name);

error:
    return NULL;
}

int Database_createCharacterStats(struct Database *db, struct CharacterRecord *stats)
{
    check(db != NULL, "Expected a non-null database.");

    struct CharacterTable *table = db->tables[CHARACTER_STATS];
    check(table != NULL, "Character stats table is not initialized.");

    stats->id = Database_getNextIndex(db, CHARACTER_STATS);
    unsigned char idx = CharacterTable_newRow(table, stats);
    return (int)idx;

error:
    return -1;
}

int Database_updateCharacterStats(struct Database *db, struct CharacterRecord *stats, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct CharacterTable *table = db->tables[CHARACTER_STATS];
    check(table != NULL, "Character stats table is not initialized.");

    unsigned char idx = CharacterTable_update(table, stats, id);

    return (int)idx;

error:
    return -1;
}

struct DialogRecord *Database_getDialog(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct DialogTable *table = db->tables[DIALOG];
    check(table != NULL, "Dialog table is not initialized.");

    return DialogTable_get(table, id);

error:
    return NULL;
}

int Database_createDialog(struct Database *db, struct DialogRecord *dialog)
{
    check(db != NULL, "Expected a non-null database.");

    struct DialogTable *table = db->tables[DIALOG];
    check(table != NULL, "Dialog table is not initialized");

    unsigned short id = DialogTable_newRow(table, dialog); // newRow takes ownership of dialog
    return (int)id;

error:
    return -1;
}

int Database_updateDialog(struct Database *db, struct DialogRecord *dialog, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct DialogTable *table = db->tables[DIALOG];
    check(table != NULL, "Dialog table is not initialized");

    check(DialogTable_update(table, dialog, id) == id, "Update returned new ID"); // update takes ownership of dialog
    return id;

error:
    return -1;
}

struct ItemRecord *Database_getItem(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    return ItemTable_get(table, id);

error:
    return NULL;
}

struct ItemRecord *Database_getItemByName(struct Database *db, char *name)
{
    check(db != NULL, "Expected a non-null database.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    return ItemTable_getByName(table, name);

error:
    return NULL;
}


struct ItemRecord *Database_getOrCreateItem(struct Database *db, char *name)
{
    struct ItemRecord *item = Database_getItemByName(db, name);
    if (item == NULL) {
        int id = Database_getNextIndex(db, ITEMS);
        item = ItemRecord_create(id, name, 0);
        Database_createItem(db, item);
        free(item);
        item = Database_getItem(db, id);
    }
    return item;
}

int Database_createItem(struct Database *db, struct ItemRecord *item)
{
    check(db != NULL, "Expected a non-null database.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    item->id = Database_getNextIndex(db, ITEMS);
    unsigned short idx = ItemTable_newRow(table, item);
    return (int)idx;

error:
    return -1;
}

int Database_updateItem(struct Database *db, struct ItemRecord *item, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    item->id = id;
    ItemTable_update(table, item);
    return id;

error:
    return -1;
}

struct DescriptionRecord *Database_getDescription(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    return DescriptionTable_get(table, id);

error:
    return NULL;
}

struct DescriptionRecord *Database_getDescriptionByPrefix(struct Database *db, char *prefix)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    return DescriptionTable_get_by_prefix(table, prefix);

error:
    return NULL;
}

struct DescriptionRecord *Database_getOrCreateDescription(struct Database *db, char *prefix)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    struct DescriptionRecord *desc = DescriptionTable_get_by_prefix(table, prefix);
    if (desc == NULL) {
        desc = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), prefix, 0);
        int idx = Database_createDescription(db, desc);
        free(desc);
        desc = DescriptionTable_get(table, idx);
    }

    return desc;

error:
    return NULL;
}

int Database_createDescription(struct Database *db, struct DescriptionRecord *desc)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    desc->id = Database_getNextIndex(db, DESCRIPTION);
    unsigned short idx = DescriptionTable_insert(table, desc);
    return (int)idx;

error:
    return -1;
}

int Database_updateDescription(struct Database *db, struct DescriptionRecord *desc, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    desc->id = id;
    DescriptionTable_update(table, desc, id);
    return id;

error:
    return -1;
}

void Database_deleteDescription(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    check(table != NULL, "Description table is not initialized.");

    DescriptionTable_delete(table, id);

error:
    return;
}

struct InventoryRecord *Database_getInventory(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    return InventoryTable_get(table, id);

error:
    return NULL;
}

struct InventoryRecord *Database_getInventoryByOwner(struct Database *db, char *owner)
{
    check(db != NULL, "Expected a non-null database.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    struct CharacterRecord *owner_record = Database_getCharacterStatsByName(db, owner);
    check(owner_record != NULL, "Character stats record not found.");

    return InventoryTable_getByOwner(table, owner_record->id);

error:
    return NULL;
}

int Database_createInventory(struct Database *db, char *owner)
{
    check(db != NULL, "Expected a non-null database.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    struct CharacterRecord *owner_record = Database_getCharacterStatsByName(db, owner);
    check(owner_record != NULL, "Character stats record not found.");

    unsigned short idx = InventoryTable_add(table, owner_record->id, Database_getNextIndex(db, INVENTORY));
    return (int)idx;

error:
    return -1;
}

int Database_updateInventory(struct Database *db, struct InventoryRecord *record, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct InventoryTable *table = db->tables[INVENTORY];
    check(table != NULL, "Inventory table is not initialized.");

    record->id = id;
    id = InventoryTable_update(table, record, id);
    return id;

error:
    return -1;
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
        items[i] = Database_getItem(db, inventory->item_ids[i]);
    }

    return items;

error:
    return NULL;
}

void Database_deleteItem(struct Database *db, int id)
{
    check(db != NULL, "Expected a non-null database.");

    struct ItemTable *table = db->tables[ITEMS];
    check(table != NULL, "Item table is not initialized.");

    ItemTable_delete(table, id);

error:
    return;
}
