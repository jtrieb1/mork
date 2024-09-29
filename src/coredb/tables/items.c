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

#include "items.h"
#include "row.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct ItemRecord *ItemRecord_create(unsigned short id, char *name, unsigned short description_id)
{
    check(id > 0, "Expected a valid ID");
    check(name != NULL && strcmp(name, "") != 0, "Expected a valid name");

    struct ItemRecord *record = calloc(1, sizeof(struct ItemRecord));
    check_mem(record);

    record->id = id;
    record->set = 0;

    int name_len = strlen(name);
    strncpy(record->name, name, MAX_NAME);
    // Fill remainder of name with null bytes
    for (int i = name_len; i < MAX_NAME; i++) {
        record->name[i] = '\0';
    }

    record->description_id = description_id;

    return record;

error:
    return NULL;
}

enum MorkResult ItemRecord_destroy(struct ItemRecord *record)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    free(record);
    return MORK_OK;
}

enum MorkResult ItemTable_init(struct ItemTable *table) {
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        memset(&table->rows[i], 0, sizeof(struct ItemRecord));
    }

    return MORK_OK;
}

struct ItemTable *ItemTable_create()
{
    struct ItemTable *table = calloc(1, sizeof(struct ItemTable));
    memset(table, 0, sizeof(struct ItemTable));
    check_mem(table);
    ItemTable_init(table);

    return table;

error:
    return NULL;
}

enum MorkResult ItemTable_destroy(struct ItemTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    free(table);
    return MORK_OK;
}

enum MorkResult ItemTable_newRow(struct ItemTable *it, struct ItemRecord *record)
{
    if (it == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    record->set = 1;
    unsigned short idx = findNextRowToFill(it->rows, MAX_ROWS_ITEMS);
    memcpy(&it->rows[idx], record, sizeof(struct ItemRecord));
    it->rows[idx].set = 1;

    return MORK_OK;
}

enum MorkResult ItemTable_update(struct ItemTable *it, struct ItemRecord *record)
{
    if (it == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    for (unsigned short i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (it->rows[i].id == record->id) {
            memcpy(&it->rows[i], record, sizeof(struct ItemRecord));
            it->rows[i].set = 1;
            return MORK_OK;
        }
    }

    return ItemTable_newRow(it, record);
}

struct ItemRecord *ItemTable_get(struct ItemTable *table, unsigned short id)
{
    check(table != NULL, "Expected a valid table, got NULL");
    check(table->rows != NULL, "Expected valid rows, got NULL");
    check(id > 0, "Expected a valid ID");

    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    log_err("Item not found (by ID).");

error:
    return NULL;
}

struct ItemRecord *ItemTable_getByName(struct ItemTable *table, char *name)
{
    check(table != NULL, "Expected a valid table, got NULL");
    check(name != NULL && strcmp(name, "") != 0, "Expected a valid name");

    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (strncmp(table->rows[i].name, name, MAX_NAME) == 0) {
            return &table->rows[i];
        }
    }

    log_err("Item not found (by name).");

error:
    return NULL;
}

enum MorkResult ItemTable_list(struct ItemTable *table)
{
    if (table == NULL) return MORK_ERROR_DB_TABLE_NULL; 

    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].set == 1) {
            log_info("ID: %d, Name: %s, Description ID: %d", table->rows[i].id, table->rows[i].name, table->rows[i].description_id);
        }
    }

    return MORK_OK;
}

enum MorkResult ItemTable_delete(struct ItemTable *table, unsigned short id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}
