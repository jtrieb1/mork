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
#include "row.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct DialogRecord *DialogRecord_default()
{
    return DialogRecord_create(0, "", 0);
}

struct DialogRecord *DialogRecord_create(unsigned short id, char *dialog, int next_id)
{
    struct DialogRecord *record = calloc(1, sizeof(struct DialogRecord));
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
    struct DialogRecord *record = DialogRecord_default();
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        memcpy(&table->rows[i], record, sizeof(struct DialogRecord));
    }
    free(record);
}

struct DialogTable *DialogTable_create()
{
    struct DialogTable *table = calloc(1, sizeof(struct DialogTable));
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

unsigned short DialogTable_newRow(struct DialogTable *table, struct DialogRecord *rec)
{
    unsigned short idx = findNextRowToFill(table->rows, MAX_ROWS_DIALOG);
    memcpy(&table->rows[idx], rec, sizeof(struct DialogRecord));
    return rec->id;
}

unsigned short DialogTable_update(struct DialogTable *table, struct DialogRecord *rec, unsigned short id)
{
    for (unsigned short i = 0; i < MAX_ROWS_DIALOG; ++i) {
        if (table->rows[i].id == id) {
            memcpy(&table->rows[i], rec, sizeof(struct DialogRecord));
            return id;
        }
    }

    return DialogTable_newRow(table, rec);
}

struct DialogRecord *DialogTable_get(struct DialogTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void DialogTable_delete(struct DialogTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

void DialogTable_print(struct DialogTable *table)
{
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1) {
            printf("ID: %d\n", table->rows[i].id);
            printf("Text: %s\n", table->rows[i].text);
            printf("Next ID: %d\n", table->rows[i].next_id);
        }
    }
}
