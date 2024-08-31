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

#include "dialog.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct DialogRecord *DialogRecord_create(unsigned short id, char *dialog, int next_id)
{
    struct DialogRecord *record = malloc(sizeof(struct DialogRecord));
    check_mem(record);

    record->id = id;
    strncpy(record->text, dialog, MAX_TEXT);
    record->text[MAX_TEXT - 1] = '\0';
    record->next_id = next_id;

    return record;

error:
    return NULL;
}

void DialogRecord_destroy(struct DialogRecord *record)
{
    free(record);
}

void DialogTable_init(struct DialogTable *table) {
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        table->rows[i].id = 0;
        table->rows[i].next_id = 0;
        table->rows[i].set = 0;
    }
}

struct DialogTable *DialogTable_create()
{
    struct DialogTable *table = malloc(sizeof(struct DialogTable));
    check_mem(table);

    DialogTable_init(table);
    return table;

error:
    return NULL;
}

void DialogTable_destroy(struct DialogTable *table)
{
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1) {
            DialogRecord_destroy(&table->rows[i]);
        }
    }
    free(table);
}

unsigned short DialogTable_set(struct DialogTable *table, struct DialogRecord *record)
{
    // See if we have a matching row
    for (unsigned short i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == record->id) {
            table->rows[i] = *record;
            return i;
        }
    }

    // If no existing row with matching id, add new row
    if (table->maxOccupiedRow < MAX_ROWS_DIALOG - 1) {
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
            for (unsigned short i = 0; i < MAX_ROWS_DIALOG - 1; i++) {
                if (table->rows[i].set == 0) {
                    table->rows[i] = *record;
                    table->maxOccupiedRow = i;
                    table->nextEmptyRow = i + 1;
                    return table->maxOccupiedRow;
                }
            }
            // No empty row found
            return -1; // This is a really unexpected scenario, so panic
        }
    } else {
        // Table is full
        unsigned char oldestRow = 0;
        // The oldest row is the row with the lowest id
        for (unsigned short i = 0; i < MAX_ROWS_DIALOG - 1; i++) {
            if (table->rows[i].id < table->rows[oldestRow].id) {
                oldestRow = i;
            }
        }
        table->rows[oldestRow] = *record;
        return oldestRow;
    }
}

struct DialogRecord *DialogTable_get(struct DialogTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void DialogTable_delete(struct DialogTable *table, unsigned short id)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

void DialogTable_print(struct DialogTable *table)
{
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].set == 1) {
            printf("ID: %d\n", table->rows[i].id);
            printf("Text: %s\n", table->rows[i].text);
            printf("Next ID: %d\n", table->rows[i].next_id);
        }
    }
}
