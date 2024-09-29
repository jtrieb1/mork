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

/**
 * @brief The preferred constructor for the DialogRecord struct.
 * 
 * @param id        The ID of the record
 * @param dialog    The dialog text
 * @param next_id   The ID of a continuation record or 0
 * @return struct DialogRecord* 
 */
struct DialogRecord *DialogRecord_create(unsigned short id, char *dialog, int next_id)
{
    check(id > 0, "Expected valid ID");
    check(dialog != NULL && strcmp(dialog, "") != 0, "Expected dialog, got empty or NULL");
    check(next_id >= 0, "Expected valid or zero next_id, got %d", next_id);

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

/**
 * @brief The destructor for the DialogRecord struct.
 * 
 * @param record 
 */
enum MorkResult DialogRecord_destroy(struct DialogRecord *record)
{
    if (!record) { return MORK_ERROR_DB_RECORD_NULL; }
    free(record);
    return MORK_OK;
}

/**
 * @brief Initialize the DialogTable struct with default values.
 * 
 * @param table 
 */
enum MorkResult DialogTable_init(struct DialogTable *table) {
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        table->rows[i].id = 0;
        table->rows[i].set = 0;
    }

    return MORK_OK;
}

/**
 * @brief The default constructor for the DialogTable struct.
 * 
 * @return struct DialogTable* 
 */
struct DialogTable *DialogTable_create()
{
    struct DialogTable *table = calloc(1, sizeof(struct DialogTable));
    check_mem(table);

    DialogTable_init(table);
    return table;

error:
    return NULL;
}

/**
 * @brief The destructor for the DialogTable struct.
 * 
 * @param table 
 */
enum MorkResult DialogTable_destroy(struct DialogTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    
    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1) {
            enum MorkResult retval = DialogRecord_destroy(&table->rows[i]);
            if (retval != MORK_OK) {
                return retval;
            }
        }
    }

    free(table);
    return MORK_OK;
}

/**
 * @brief Find the next row to fill in the table.
 * 
 * @param rows The rows to search
 * @param max The maximum number of rows
 * @return unsigned short 
 */
enum MorkResult DialogTable_newRow(struct DialogTable *table, struct DialogRecord *rec)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (rec == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    unsigned short idx = findNextRowToFill(table->rows, MAX_ROWS_DIALOG);
    memcpy(&table->rows[idx], rec, sizeof(struct DialogRecord));
    table->rows[idx].set = 1;
    return MORK_OK;
}

/**
 * @brief Update a Dialog in the table.
 * 
 * @param table 
 * @param rec 
 * @param id 
 * @return unsigned short 
 */
enum MorkResult DialogTable_update(struct DialogTable *table, struct DialogRecord *rec)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (rec == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    for (unsigned short i = 0; i < MAX_ROWS_DIALOG; ++i) {
        if (table->rows[i].set == 1 && table->rows[i].id == rec->id) {
            memcpy(&table->rows[i], rec, sizeof(struct DialogRecord));
            table->rows[i].set = 1;
            return MORK_OK;
        }
    }

    return DialogTable_newRow(table, rec);
}

/**
 * @brief Get a Dialog from the table.
 * 
 * @param table 
 * @param id 
 * @return struct DialogRecord* 
 */
struct DialogRecord *DialogTable_get(struct DialogTable *table, unsigned short id)
{
    check(table != NULL, "Expected table, got NULL");
    check(id > 0, "Expected valid ID, got 0");

    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    log_err("Dialog ID %d not found", id);

error:
    return NULL;
}

/**
 * @brief Delete a Dialog from the table.
 * 
 * @param table 
 * @param id 
 */
enum MorkResult DialogTable_delete(struct DialogTable *table, unsigned short id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1 && table->rows[i].id == id) {
            table->rows[i].set = 0;
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_NOT_FOUND;
}

/**
 * @brief Print all records in the table
 * 
 * @param table 
 */
enum MorkResult DialogTable_print(struct DialogTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_ROWS_DIALOG; i++) {
        if (table->rows[i].set == 1) {
            printf("ID: %d\n", table->rows[i].id);
            printf("Text: %s\n", table->rows[i].text);
            printf("Next ID: %d\n", table->rows[i].next_id);
        }
    }
    return MORK_OK;
}
