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

#include "inventory.h"
#include "row.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct InventoryRecord* InventoryRecord_create(unsigned short id, unsigned short owner_id)
{
    check(id > 0, "Expected a valid ID");
    check(owner_id > 0, "Expected a valid Owner ID");

    struct InventoryRecord* record = calloc(1, sizeof(struct InventoryRecord));
    check_mem(record);

    record->id = id;
    record->set = 1;
    record->owner_id = owner_id;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        record->item_ids[i] = 0;
    }

    return record;

error:
    return NULL;
}

enum MorkResult InventoryRecord_destroy(struct InventoryRecord* record)
{
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    free(record);
    return MORK_OK;
}

enum MorkResult InventoryRecord_addItem(struct InventoryRecord* record, unsigned short item_id)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (item_id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == 0) {
            record->item_ids[i] = item_id;
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_FIELD_FULL;
}

enum MorkResult InventoryRecord_removeItem(struct InventoryRecord* record, unsigned short item_id)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (item_id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == item_id) {
            record->item_ids[i] = 0;
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_NOT_FOUND;
}

unsigned short InventoryRecord_getItemCount(struct InventoryRecord* record)
{
    if (record == NULL) return 0;

    unsigned short count = 0;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            count++;
        }
    }
    return count;
}

unsigned short InventoryRecord_getID(struct InventoryRecord* record)
{
    if (record == NULL) return 0;
    return record->id;
}

unsigned short InventoryRecord_getOwnerID(struct InventoryRecord* record)
{
    if (record == NULL) return 0;
    return record->owner_id;
}

enum MorkResult InventoryTable_init(struct InventoryTable* table)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        memset(&table->rows[i], 0, sizeof(struct InventoryRecord));
    }
    
    return MORK_OK;
}

struct InventoryTable* InventoryTable_create()
{
    struct InventoryTable* table = calloc(1, sizeof(struct InventoryTable));
    check_mem(table);

    InventoryTable_init(table);
    return table;

error:
    return NULL;
}

enum MorkResult InventoryTable_destroy(struct InventoryTable* table)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }
    free(table);
    return MORK_OK;
}

enum MorkResult InventoryTable_add(struct InventoryTable *table, unsigned short owner_id, int junction_id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (owner_id == 0) { return MORK_ERROR_DB_INVALID_ID; }
    if (junction_id <= 0) {
        log_err("Invalid Junction ID %d", junction_id);
        return MORK_ERROR_DB_INVALID_ID;
    }

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].set == 0 && table->rows[i].id == 0) {
            table->rows[i].set = 1;
            table->rows[i].id = junction_id;
            table->rows[i].owner_id = owner_id;
            for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                table->rows[i].item_ids[j] = 0;
            }
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_TABLE_FULL;
}

enum MorkResult InventoryTable_update(struct InventoryTable *table, struct InventoryRecord *record)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == record->id) {
            table->rows[i].owner_id = record->owner_id;
            for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                table->rows[i].item_ids[j] = record->item_ids[j];
            }
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_NOT_FOUND;
}

enum MorkResult InventoryTable_remove(struct InventoryTable *table, unsigned short id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == id) {
            table->rows[i].set = 0;
            table->rows[i].id = 0;
            table->rows[i].owner_id = 0;
            for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                table->rows[i].item_ids[j] = 0;
            }
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_NOT_FOUND;
}

struct InventoryRecord* InventoryTable_get(struct InventoryTable *table, unsigned short id)
{
    check(table != NULL, "Expected a valid table, got NULL");
    check(id != 0, "Expected a valid ID");

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

error:
    return NULL;
}

struct InventoryRecord* InventoryTable_getByOwner(struct InventoryTable* table, unsigned short owner_id)
{
    check(table != NULL, "Expected valid table, got NULL");
    check(owner_id > 0, "Expected valid Owner ID");

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].set == 1 && table->rows[i].owner_id == owner_id) {
            return &table->rows[i];
        }
    }

error:
    return NULL;
}

enum MorkResult InventoryTable_print(struct InventoryTable* table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        struct InventoryRecord* row = &table->rows[i];
        if (row->set == 1) {
            log_info("ID: %d, Owner ID: %d", row->id, row->owner_id);
            for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                if (row->item_ids[j] != 0) {
                    log_info("Item ID: %d", row->item_ids[j]);
                }
            }
        }
    }
    return MORK_OK;
}
