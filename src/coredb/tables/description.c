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

#include "description.h"

#include <string.h>
#include <lcthw/dbg.h>
#include <stdlib.h>

struct DescriptionRecord *DescriptionRecord_create(unsigned short id, char *description, int next_id)
{
    struct DescriptionRecord *entry = malloc(sizeof(struct DescriptionRecord));
    check_mem(entry);

    entry->id = id;
    strncpy(entry->description, description, MAX_DESCRIPTION);
    entry->description[MAX_DESCRIPTION - 1] = '\0';
    entry->next_id = next_id;

    return entry;

error:
    return NULL;
}

void DescriptionRecord_destroy(struct DescriptionRecord *entry)
{
    free(entry);
}

void DescriptionTable_init(struct DescriptionTable *table) {
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        table->rows[i].id = 0;
        table->rows[i].next_id = 0;
        table->rows[i].set = 0;
    }
}

struct DescriptionTable *DescriptionTable_create()
{
    struct DescriptionTable *table = malloc(sizeof(struct DescriptionTable));
    check_mem(table);

    DescriptionTable_init(table);
    return table;

error:
    return NULL;
}

void DescriptionTable_destroy(struct DescriptionTable *table)
{
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].set == 1) {
            DescriptionRecord_destroy(&table->rows[i]);
        }
    }
    free(table);
}

unsigned short DescriptionTable_set(struct DescriptionTable *table, struct DescriptionRecord *entry)
{
    // Check for existing row with matching id
    for (unsigned short i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == entry->id) {
            table->rows[i] = *entry;
            return i;
        }
    }

    // If no existing row with matching id, add new row
    if (table->maxOccupiedRow < MAX_ROWS_DESC - 1) {
        // Table is not full here
        // Find where the row needs to go
        // Is nextEmptyRow empty?
        if (table->rows[table->nextEmptyRow].set == 0) {
            table->rows[table->nextEmptyRow] = *entry;
            table->maxOccupiedRow = table->nextEmptyRow;
            table->nextEmptyRow++;
            return table->maxOccupiedRow;
        } else {
            // Find the next empty row
            for (unsigned short i = 0; i < MAX_ROWS_DESC - 1; i++) {
                if (table->rows[i].set == 0) {
                    table->rows[i] = *entry;
                    table->maxOccupiedRow = i;
                    table->nextEmptyRow = i + 1;
                    return table->maxOccupiedRow;
                }
            }
            // No empty row found, this is highly unlikely but not a complete failure
        }
    }

    // Table is full
    unsigned short oldestRow = 0;
    // The oldest row is the row with the lowest id
    for (unsigned short i = 0; i < MAX_ROWS_DESC - 1; i++) {
        if (table->rows[i].id < table->rows[oldestRow].id) {
            oldestRow = i;
        }
    }
    table->rows[oldestRow] = *entry;
    return oldestRow;

}

struct DescriptionRecord *DescriptionTable_get(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    return NULL;
}

struct DescriptionRecord *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[table->rows[i].next_id];
        }
    }

    return NULL;
}

struct DescriptionRecord *DescriptionTable_get_by_prefix(struct DescriptionTable *table, char *prefix)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (strncmp(table->rows[i].description, prefix, strlen(prefix)) == 0) {
            return &table->rows[i];
        }
    }

    return NULL;
}

void DescriptionTable_delete(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}
