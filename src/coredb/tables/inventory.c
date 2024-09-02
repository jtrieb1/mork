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

struct InventoryRecord *InventoryRecord_default()
{
    return InventoryRecord_create(0, 0);
}

struct InventoryRecord* InventoryRecord_create(unsigned short id, unsigned short owner_id)
{
    struct InventoryRecord* record = calloc(1, sizeof(struct InventoryRecord));
    check_mem(record);

    record->id = id;
    record->owner_id = owner_id;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        record->item_ids[i] = 0;
    }

    return record;

error:
    return NULL;
}

void InventoryRecord_destroy(struct InventoryRecord* record)
{
    free(record);
}

void InventoryRecord_addItem(struct InventoryRecord* record, unsigned short item_id)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == 0) {
            record->item_ids[i] = item_id;
            break;
        }
    }
}

void InventoryRecord_removeItem(struct InventoryRecord* record, unsigned short item_id)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == item_id) {
            record->item_ids[i] = 0;
            break;
        }
    }
}

unsigned short InventoryRecord_getItemCount(struct InventoryRecord* record)
{
    unsigned short count = 0;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            count++;
        }
    }
    return count;
}

unsigned short InventoryRecord_getItem(struct InventoryRecord* record, unsigned short index)
{
    return record->item_ids[index];
}

unsigned short InventoryRecord_getID(struct InventoryRecord* record)
{
    return record->id;
}

unsigned short InventoryRecord_getOwnerID(struct InventoryRecord* record)
{
    return record->owner_id;
}

void InventoryTable_init(struct InventoryTable* table)
{
    struct InventoryRecord *record = InventoryRecord_default();
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        memcpy(&table->rows[i], record, sizeof(struct InventoryRecord));
    }
    free(record);
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

void InventoryTable_destroy(struct InventoryTable* table)
{
    free(table);
}

unsigned short InventoryTable_add(struct InventoryTable *table, unsigned short owner_id, int junction_id)
{
    unsigned short id = 0;
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == 0) {
            id = findNextRowToFill(table->rows, MAX_ROWS_INVENTORIES);
            table->rows[i].id = junction_id;
            table->rows[i].owner_id = owner_id;
            break;
        }
    }
    return id;
}

unsigned short InventoryTable_update(struct InventoryTable *table, struct InventoryRecord *record, int id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == id) {
            memcpy(&table->rows[i], record, sizeof(struct InventoryRecord));
            return id;
        }
    }
    return 0;
}

void InventoryTable_remove(struct InventoryTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].id = 0;
            table->rows[i].owner_id = 0;
            for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                table->rows[i].item_ids[j] = 0;
            }
            break;
        }
    }
}

struct InventoryRecord* InventoryTable_get(struct InventoryTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

struct InventoryRecord* InventoryTable_getByOwner(struct InventoryTable* table, unsigned short owner_id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].owner_id == owner_id) {
            return &table->rows[i];
        }
    }
    return NULL;
}
