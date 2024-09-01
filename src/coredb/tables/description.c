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

struct DescriptionRecord *DescriptionRecord_default()
{
    return DescriptionRecord_create(0, "", 0);
}

struct DescriptionRecord *DescriptionRecord_create(unsigned short id, char *description, int next_id)
{
    struct DescriptionRecord *entry = calloc(1, sizeof(struct DescriptionRecord));
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
    entry = NULL;
}

void DescriptionTable_init(struct DescriptionTable *table) {
    struct DescriptionRecord *record = DescriptionRecord_default();
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        memcpy(&table->rows[i], record, sizeof(struct DescriptionRecord));
    }
    free(record);
    record = NULL;
}

struct DescriptionTable *DescriptionTable_create()
{
    struct DescriptionTable *table = calloc(1, sizeof(struct DescriptionTable));
    check_mem(table);

    DescriptionTable_init(table);
    return table;

error:
    return NULL;
}

void DescriptionTable_destroy(struct DescriptionTable *table)
{
    free(table);
}

static unsigned short findNextRowToFill(struct DescriptionTable *table)
{
    unsigned short idx = 0;
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == 0) {
            idx = i;
            break;
        }
    }
    if (idx == 0) {
        // No empty rows, so find oldest and overwrite
        int min_id = 65535;
        for (int i = 0; i < MAX_ROWS_DESC; i++) {
            if (table->rows[i].id < min_id) {
                min_id = table->rows[i].id;
                idx = i;
            }
        }
    }
    return idx;
}

unsigned short DescriptionTable_insert(struct DescriptionTable *table, struct DescriptionRecord *record)
{
    unsigned short idx = findNextRowToFill(table);
    memcpy(&table->rows[idx], record, sizeof(struct DescriptionRecord));
    table->rows[idx].set = 1;
    return table->rows[idx].id;
}

unsigned short DescriptionTable_update(struct DescriptionTable *table, struct DescriptionRecord *record, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == id) {
            memcpy(&table->rows[i], record, sizeof(struct DescriptionRecord));
            table->rows[i].set = 1;
            return table->rows[i].id;
        }
    }

    return 0;
}

struct DescriptionRecord *DescriptionTable_get(struct DescriptionTable *table, unsigned short id)
{
    check(id > 0, "ID is not set");
    check(table != NULL, "Table is NULL");

    for (unsigned short i = 0; i < MAX_ROWS_DESC; i++) {
        struct DescriptionRecord *rec = &table->rows[i];
        if (rec->id == id) {
            return rec;
        }
    }

error:
    return NULL;
}

struct DescriptionRecord *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[table->rows[i].next_id];
        }
    }

    return NULL;
}

struct DescriptionRecord *DescriptionTable_get_by_prefix(struct DescriptionTable *table, char *prefix)
{
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (strncmp(table->rows[i].description, prefix, strlen(prefix)) == 0) {
            return &table->rows[i];
        }
    }

    return NULL;
}

void DescriptionTable_delete(struct DescriptionTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}
