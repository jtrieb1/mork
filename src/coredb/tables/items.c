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

#include <lcthw/dbg.h>
#include <stdlib.h>

struct ItemRecord *ItemRecord_create(unsigned short id, char *name, unsigned short description_id)
{
    struct ItemRecord *record = malloc(sizeof(struct ItemRecord));
    check_mem(record);

    record->id = id;
    record->set = 1;
    strncpy(record->name, name, MAX_NAME);
    record->name[MAX_NAME - 1] = '\0';
    record->description_id = description_id;

    return record;

error:
    return NULL;
}

void ItemRecord_destroy(struct ItemRecord *record)
{
    free(record);
}

void ItemTable_init(struct ItemTable *table) {
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        table->rows[i].id = 0;
        table->rows[i].description_id = 0;
        table->rows[i].set = 0;
    }
}

struct ItemTable *ItemTable_create()
{
    struct ItemTable *table = malloc(sizeof(struct ItemTable));
    check_mem(table);

    ItemTable_init(table);
    return table;

error:
    return NULL;
}

void ItemTable_destroy(struct ItemTable *table)
{
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].set == 1) {
            ItemRecord_destroy(&table->rows[i]);
        }
    }
    free(table);
}

unsigned short ItemTable_set(struct ItemTable *table, struct ItemRecord *record)
{
    // Check for matching record
    for (unsigned short i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == record->id) {
            // If matching record, update row
            table->rows[i] = *record;
            return i;
        }
    }

    // If no existing row with matching id, add new row
    if (table->maxOccupiedRow < MAX_ROWS_ITEMS - 1) {
        // Table is not full here
        // Find where the row needs to go
        // Is nextEmptyRow empty?
        if (table->rows[table->nextEmptyRow].set == 0) {
            table->rows[table->nextEmptyRow] = *record;
            table->maxOccupiedRow = table->nextEmptyRow;
            table->nextEmptyRow++;
            return table->maxOccupiedRow;
        } else {
            // Find the next empty row
            for (unsigned short i = 0; i < MAX_ROWS_ITEMS - 1; i++) {
                if (table->rows[i].set == 0) {
                    table->rows[i] = *record;
                    table->maxOccupiedRow = i;
                    table->nextEmptyRow = i + 1;
                    return table->maxOccupiedRow;
                }
            }
        }
    }

    // Table is full
    unsigned char oldestRow = 0;
    // The oldest row is the row with the lowest id
    for (unsigned short i = 0; i < MAX_ROWS_ITEMS - 1; i++) {
        if (table->rows[i].id < table->rows[oldestRow].id) {
            oldestRow = i;
        }
    }
    table->rows[oldestRow] = *record;
    return oldestRow;
}


struct ItemRecord *ItemTable_get(struct ItemTable *table, unsigned short id)
{
    for (unsigned short i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    return NULL;
}

struct ItemRecord *ItemTable_get_by_name(struct ItemTable *table, char *name)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (strncmp(table->rows[i].name, name, MAX_NAME) == 0) {
            return &table->rows[i];
        }
    }

    return NULL;
}

void ItemTable_list(struct ItemTable *table)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].set == 1) {
            log_info("ID: %d, Name: %s, Description ID: %d", table->rows[i].id, table->rows[i].name, table->rows[i].description_id);
        }
    }
}

void ItemTable_delete(struct ItemTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}
