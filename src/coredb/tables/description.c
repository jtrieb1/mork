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
#include "row.h"

#include <string.h>
#include <lcthw/dbg.h>
#include <stdlib.h>

/**
 * @brief The preferred constructor for the DescriptionRecord struct.
 * 
 * @param id            The ID of the record
 * @param description   The description 
 * @param next_id       The ID of a continuation record or 0
 * @return struct DescriptionRecord* 
 */
struct DescriptionRecord *DescriptionRecord_create(unsigned short id, char *description, int next_id)
{
    check(id > 0, "Expected a valid id, got %u", id);
    check(description != NULL && strcmp(description, "") != 0, "Expected a valid description, received empty");
    check(next_id >= 0, "Expected either no next ID or a valid one, got %d", next_id);

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

/**
 * @brief The destructor for the DescriptionRecord struct.
 * 
 * @param entry The record to destroy
 */
enum MorkResult DescriptionRecord_destroy(struct DescriptionRecord *entry)
{
    if (entry == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    free(entry);
    return MORK_OK;
}

/**
 * @brief Initialize the DescriptionTable with default records.
 * 
 * @param table The table to initialize
 */
enum MorkResult DescriptionTable_init(struct DescriptionTable *table) {
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        table->rows[i].id = 0;
        table->rows[i].set = 0;
    }
    return MORK_OK;
}

/**
 * @brief Create a new DescriptionTable.
 * 
 * @return struct DescriptionTable* 
 */
struct DescriptionTable *DescriptionTable_create()
{
    struct DescriptionTable *table = calloc(1, sizeof(struct DescriptionTable));
    check_mem(table);

    DescriptionTable_init(table);
    return table;

error:
    return NULL;
}

/**
 * @brief Destroy a DescriptionTable.
 * 
 * @param table The table to destroy
 */
enum MorkResult DescriptionTable_destroy(struct DescriptionTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    free(table);
    return MORK_OK;
}

/**
 * @brief Insert a record into the DescriptionTable.
 * 
 * @param table  The table to insert into
 * @param record The record to insert
 * @return unsigned short The ID of the inserted record
 */

enum MorkResult DescriptionTable_insert(struct DescriptionTable *table, struct DescriptionRecord *record)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    record->set = 1;
    unsigned short idx = findNextRowToFill(table->rows, MAX_ROWS_DESC);

    memcpy(&table->rows[idx], record, sizeof(struct DescriptionRecord));
    table->rows[idx].set = 1;

    return MORK_OK;
}

/**
 * @brief Update a record in the DescriptionTable.
 * 
 * @param table   The table to update
 * @param record  The record to update
 * @param id      The ID of the record to update, may differ from the id in record
 * @return unsigned short 
 */
enum MorkResult DescriptionTable_update(struct DescriptionTable *table, struct DescriptionRecord *record)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == record->id) {
            memcpy(&table->rows[i], record, sizeof(struct DescriptionRecord));
            table->rows[i].set = 1;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}

/**
 * @brief Get a record from the DescriptionTable by ID.
 * 
 * @param table The table to search
 * @param id    The ID of the record to find
 * @return struct DescriptionRecord* 
 */
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

/**
 * @brief Finds the description with the associated ID, then returns the next description in the chain.
 * 
 * @param table The table to search
 * @param id    The ID of the current record
 * @return struct DescriptionRecord* 
 */
struct DescriptionRecord *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id)
{
    check(table != NULL, "Expected table, got NULL");
    check(id != 0, "Expected valid ID");

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[table->rows[i].next_id];
        }
    }

error:
    return NULL;
}

/**
 * @brief Attempts to find a DescriptionRecord by matching against content.
 * 
 * @param table The table to search
 * @param prefix The prefix to search for
 * @return struct DescriptionRecord* 
 */
struct DescriptionRecord *DescriptionTable_get_by_prefix(struct DescriptionTable *table, char *prefix)
{
    check(table != NULL, "Expected table, got NULL");
    check(prefix != NULL && strcmp(prefix, "") != 0, "Expected valid prefix, got empty or NULL");

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (strncmp(table->rows[i].description, prefix, strlen(prefix)) == 0) {
            return &table->rows[i];
        }
    }

error:
    return NULL;
}

/**
 * @brief Deletes a record from the DescriptionTable.
 * 
 * @param table The table to delete from
 * @param id    The ID of the record to delete
 */
enum MorkResult DescriptionTable_delete(struct DescriptionTable *table, unsigned short id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return MORK_OK;
        }
    }
    return MORK_ERROR_DB_NOT_FOUND;
}

enum MorkResult DescriptionTable_print(struct DescriptionTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_ROWS_DESC; i++) {
        struct DescriptionRecord *rec = &table->rows[i];
        if (rec->set == 1) {
            log_info("ID: %d, Description: %s, Next ID: %d", rec->id, rec->description, rec->next_id);
        }
    }

    return MORK_OK;
}
