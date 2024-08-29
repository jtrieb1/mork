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

void Database_open(struct Database *db, const char *path)
{
    assert(db != NULL);
    assert(path != NULL);

    db->file = fopen(path, "r+");
    check(db->file, "Failed to open file: %s", path);

error:
    return;
}

void Database_close(struct Database *db)
{
    assert(db != NULL);

    if (db->file) {
        // Make sure we write out to the file before closing it
        for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
            if (db->tables[tbl]) {
                Database_write(db, tbl);
            }
        }
        fclose(db->file);
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

    for (enum Table tbl = 0; tbl < MAX_TABLES; tbl++) {
        switch (tbl) {
            case CHARACTER_STATS:
                CharacterStatsTable_destroy(db->tables[tbl]);
                break;
            case DIALOG:
                DialogTable_destroy(db->tables[tbl]);
                break;
            case ITEMS:
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
    assert(db != NULL);

    struct CharacterStatsTable *table = db->tables[CHARACTER_STATS];
    assert(table != NULL);

    CharacterStatsTable_set(table, stats);
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
    assert(db != NULL);

    struct DialogTable *table = db->tables[DIALOG];
    assert(table != NULL);

    DialogTable_set(table, dialog);
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
    assert(db != NULL);

    struct ItemTable *table = db->tables[ITEMS];
    assert(table != NULL);

    ItemTable_set(table, item);
}
