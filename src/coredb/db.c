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
#include "tables/character_stats.h"
#include "tables/description.h"
#include "tables/dialog.h"
#include "tables/inventory.h"

#include <assert.h>
#include <stdio.h>
#include <lcthw/dbg.h>

void Database_init(struct Database *db)
{
    // Initialize all tables
    db->tables[CHARACTER_STATS] = CharacterStatsTable_create();
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
                offset += sizeof(struct CharacterStatsTable);
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
            return sizeof(struct CharacterStatsTable);
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
    struct Database *db = malloc(sizeof(struct Database));
    check_mem(db);

    db->file = NULL;

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
                CharacterStatsTable_destroy(db->tables[tbl]);
                break;
            case DESCRIPTION:
                if (db->tables[tbl] == NULL) break;
                DescriptionTable_destroy(db->tables[tbl]);
                break;
            case DIALOG:
                if (db->tables[tbl] == NULL) break;
                DialogTable_destroy(db->tables[tbl]);
                break;
            case ITEMS:
                if (db->tables[tbl] == NULL) break;
                ItemTable_destroy(db->tables[tbl]);
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
    assert(db != NULL);
    assert(table >= 0 && table < MAX_TABLES);

    return db->tables[table];
}

void Database_set(struct Database *db, enum Table table, void *data)
{
    assert(db != NULL);
    assert(table >= 0 && table < MAX_TABLES);

    db->tables[table] = data;
}

void Database_write(struct Database *db, enum Table table)
{
    assert(db != NULL);
    assert(table >= 0 && table < MAX_TABLES);

    if (db->file) {
        // We know the sizes of the individual tables, so we can write them directly via offset writes
        switch (table) {
            case CHARACTER_STATS:
                fseek(db->file, 0, SEEK_SET);
                fwrite(db->tables[table], sizeof(struct CharacterStatsTable), 1, db->file);
                break;
            case DIALOG:
                fseek(db->file, sizeof(struct CharacterStatsTable), SEEK_SET);
                fwrite(db->tables[table], sizeof(struct DialogTable), 1, db->file);
                break;
            case ITEMS:
                fseek(db->file, sizeof(struct CharacterStatsTable) + sizeof(struct DialogTable), SEEK_SET);
                fwrite(db->tables[table], sizeof(struct ItemTable), 1, db->file);
                break;
            default:
                sentinel("Unknown table: %d", table);
        }
    }

error:
    return;
}

struct CharacterStatsRecord *Database_get_character_stats(struct Database *db, int id)
{
    assert(db != NULL);

    struct CharacterStatsTable *table = db->tables[CHARACTER_STATS];
    assert(table != NULL);

    return CharacterStatsTable_get(table, id);
}

void Database_set_character_stats(struct Database *db, struct CharacterStatsRecord *stats)
{
    // This method takes ownership of the stats record, so it must be freed here
    assert(db != NULL);

    struct CharacterStatsTable *table = db->tables[CHARACTER_STATS];
    assert(table != NULL);

    CharacterStatsTable_set(table, stats);
    CharacterStatsRecord_destroy(stats);
}

struct DialogRecord *Database_get_dialog(struct Database *db, int id)
{
    assert(db != NULL);

    struct DialogTable *table = db->tables[DIALOG];
    assert(table != NULL);

    return DialogTable_get(table, id);
}

void Database_set_dialog(struct Database *db, struct DialogRecord *dialog)
{
    // This method takes ownership of the dialog record, so it must be freed here
    assert(db != NULL);

    struct DialogTable *table = db->tables[DIALOG];
    assert(table != NULL);

    DialogTable_set(table, dialog);
    DialogRecord_destroy(dialog);
}

struct ItemRecord *Database_get_item(struct Database *db, int id)
{
    assert(db != NULL);

    struct ItemTable *table = db->tables[ITEMS];
    assert(table != NULL);

    return ItemTable_get(table, id);
}

void Database_set_item(struct Database *db, struct ItemRecord *item)
{
    // This method takes ownership of the item record, so it must be freed here
    assert(db != NULL);

    struct ItemTable *table = db->tables[ITEMS];
    assert(table != NULL);

    ItemTable_set(table, item);
    ItemRecord_destroy(item);
}

struct DescriptionRecord *Database_get_description(struct Database *db, int id)
{
    assert(db != NULL);

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    assert(table != NULL);

    return DescriptionTable_get(table, id);
}

void Database_set_description(struct Database *db, struct DescriptionRecord *desc)
{
    // This method takes ownership of the description record, so it must be freed here
    assert(db != NULL);

    struct DescriptionTable *table = db->tables[DESCRIPTION];
    assert(table != NULL);

    DescriptionTable_set(table, desc);
    DescriptionRecord_destroy(desc);
}
