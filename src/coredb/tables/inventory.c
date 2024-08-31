#include "inventory.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct InventoryRecord* InventoryRecord_create(unsigned short id, unsigned short owner_id)
{
    struct InventoryRecord* record = malloc(sizeof(struct InventoryRecord));
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

void InventoryRecord_add_item(struct InventoryRecord* record, unsigned short item_id)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == 0) {
            record->item_ids[i] = item_id;
            break;
        }
    }
}

void InventoryRecord_remove_item(struct InventoryRecord* record, unsigned short item_id)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] == item_id) {
            record->item_ids[i] = 0;
            break;
        }
    }
}

unsigned short InventoryRecord_get_item_count(struct InventoryRecord* record)
{
    unsigned short count = 0;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            count++;
        }
    }
    return count;
}

unsigned short InventoryRecord_get_item(struct InventoryRecord* record, unsigned short index)
{
    return record->item_ids[index];
}

unsigned short InventoryRecord_get_id(struct InventoryRecord* record)
{
    return record->id;
}

unsigned short InventoryRecord_get_owner_id(struct InventoryRecord* record)
{
    return record->owner_id;
}

void InventoryTable_init(struct InventoryTable* table)
{
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        table->rows[i].id = 0;
        table->rows[i].owner_id = 0;
        for (int j = 0; j < MAX_INVENTORY_ITEMS; j++) {
            table->rows[i].item_ids[j] = 0;
        }
    }
}

struct InventoryTable* InventoryTable_create()
{
    struct InventoryTable* table = malloc(sizeof(struct InventoryTable));
    check_mem(table);

    InventoryTable_init(table);
    return table;

error:
    return NULL;
}

void InventoryTable_destroy(struct InventoryTable* table)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id != 0) {
            InventoryRecord_destroy(&table->rows[i]);
        }
    }
    free(table);
}

unsigned short InventoryTable_add(struct InventoryTable *table, unsigned short owner_id)
{
    unsigned short id = 0;
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == 0) {
            id = table->nextEmptyRow++;
            table->rows[i].id = id;
            table->rows[i].owner_id = owner_id;
            break;
        }
    }
    return id;
}

void InventoryTable_remove(struct InventoryTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].id == id) {
            InventoryRecord_destroy(&table->rows[i]);
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

struct InventoryRecord* InventoryTable_get_by_owner(struct InventoryTable* table, unsigned short owner_id)
{
    for (int i = 0; i < MAX_ROWS_INVENTORIES; i++) {
        if (table->rows[i].owner_id == owner_id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

unsigned short InventoryTable_set(struct InventoryTable *table, struct InventoryRecord *record)
{
    table->rows[record->id] = *record;
    return record->id;
}
