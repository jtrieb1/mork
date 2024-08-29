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

struct DescriptionEntry *DescriptionEntry_create(unsigned short id, char *description, int next_id)
{
    struct DescriptionEntry *entry = malloc(sizeof(struct DescriptionEntry));
    check_mem(entry);

    entry->id = id;
    strncpy(entry->description, description, MAX_DESCRIPTION);
    entry->description[MAX_DESCRIPTION - 1] = '\0';
    entry->next_id = next_id;

    return entry;

error:
    return NULL;
}

void DescriptionEntry_destroy(struct DescriptionEntry *entry)
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
            DescriptionEntry_destroy(&table->rows[i]);
        }
    }
    free(table);
}

void DescriptionTable_set(struct DescriptionTable *table, struct DescriptionEntry *entry)
{
    // Check for existing row with matching id
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == entry->id) {
            table->rows[i] = *entry;
            return;
        }
    }

    // If no existing row with matching id, add new row
    if (table->maxOccupiedRow <= MAX_ROWS_DESC - 1) {
        // If table is not full, add new row to end
        table->rows[table->maxOccupiedRow] = *entry;
        // Update maxOccupiedRow and nextEmptyRow
        table->maxOccupiedRow++;
        table->nextEmptyRow = table->maxOccupiedRow;
    } else {
        // If table is full, write to next empty row and update nextEmptyRow
        // nextEmptyRow is either the oldest row or a row that was deleted
        table->rows[table->nextEmptyRow] = *entry;
        // Check if next row is empty
        table->nextEmptyRow++; // We allow overflow here since the type is unsigned
        if (table->rows[table->nextEmptyRow].set == 1) {
            // If next row is not empty, find the next empty row
            for (int i = 0; i < MAX_ROWS_DESC; i++) {
                if (table->rows[i].set == 0) {
                    table->nextEmptyRow = i;
                    break;
                }
            }
        }
    }
}

struct DescriptionEntry *DescriptionTable_get(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    return NULL;
}

struct DescriptionEntry *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[table->rows[i].next_id];
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
