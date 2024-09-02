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

#include "character.h"
#include "row.h"

#include <lcthw/dbg.h>

/**
 * @brief Calculates the index of the next row to fill in the table. If no empty rows are found, the
 *        index of the row with the lowest ID is chosen.
 * 
 * @param table 
 * @return unsigned short The index of the next row to fill
 */

/**
 * @brief Quick and dirty way to create a default character record.
 * 
 * @return struct CharacterRecord* 
 */
struct CharacterRecord *CharacterRecord_default() {
    return CharacterRecord_create(
        "", 0, 0, 0, 0, 0, 0, 0
    );
}

/**
 * @brief The preferred constructor for the CharacterRecord struct.
 * 
 * @param name        
 * @param level 
 * @param health 
 * @param maxHealth 
 * @param mana 
 * @param maxMana 
 * @param stats       An array of stats with length numStats
 * @param numStats    The length of the stats array
 * @return struct CharacterRecord* 
 */
struct CharacterRecord *CharacterRecord_create(
    char *name,
    unsigned int level,
    unsigned int health,
    unsigned int maxHealth,
    unsigned int mana,
    unsigned int maxMana,
    unsigned char *stats,
    unsigned int numStats
) {
    struct CharacterRecord *record = (struct CharacterRecord *)calloc(1, sizeof(struct CharacterRecord));
    check_mem(record);
    record->id = 0;
    record->set = 0;
    strncpy(record->name, name, MAX_NAME_LEN - 1);
    record->level = level;
    record->experience = 0;
    record->health_and_mana = SET_HEALTH_AND_MANA(health, mana);
    record->max_health_and_mana = SET_HEALTH_AND_MANA(maxHealth, maxMana);

    record->numStats = numStats;
    record->stats = 0;
    for (unsigned int i = 0; i < numStats; i++) {
        record->stats = SET_STAT(record->stats, i, stats[i]);
    }

    return record;

error:
    return NULL;
}

/**
 * @brief The preferred destructor for the CharacterRecord struct.
 * 
 * @param record The record to destroy
 */
void CharacterRecord_destroy(struct CharacterRecord *record) {
    free(record);
}

/**
 * @brief Print a character record to the console.
 * 
 * @param record The record to print
 */
void CharacterRecord_print(struct CharacterRecord *record) {
    printf("Character Stats Record\n");
    printf("ID: %d\n", record->id);
    printf("Set: %d\n", record->set);
    printf("Name: %s\n", record->name);
    printf("Level: %d\n", record->level);
    printf("Experience: %lu\n", record->experience);
    printf("Health: %lu\n", GET_HEALTH(record->health_and_mana));
    printf("Max Health: %lu\n", GET_HEALTH(record->max_health_and_mana));
    printf("Mana: %lu\n", GET_MANA(record->health_and_mana));
    printf("Max Mana: %lu\n", GET_MANA(record->max_health_and_mana));
    printf("Stats: ");
    for (int i = 0; i < 16; i++) { // Only 16 stats fit in a 64-bit unsigned long long
        unsigned long long stat = GET_STAT(record->stats, i);
        if (stat == 0) {
            break;
        }
        printf("%llu ", GET_STAT(record->stats, i));
    }
}

/**
 * @brief The constructor for the CharacterTable struct.
 * 
 * @return struct CharacterTable* 
 */
struct CharacterTable *CharacterTable_create() {
    struct CharacterTable *table = (struct CharacterTable *)calloc(1, sizeof(struct CharacterTable));
    check_mem(table);
    CharacterTable_init(table);
    return table;

error:
    return NULL;
}

/**
 * @brief Initialize a CharacterTable struct with default values.
 * 
 * @param table The table to initialize
 */
void CharacterTable_init(struct CharacterTable *table) {
    struct CharacterRecord *record = CharacterRecord_default();
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        memcpy(&table->rows[i], record, sizeof(struct CharacterRecord));
    }
    free(record);
}


/**
 * @brief Add a new row to the table. If the table is full, the row with the lowest ID will be overwritten.
 * 
 * @param table  The table to add the row to
 * @param record The record to add
 * @return unsigned char The ID of the new row
 */
unsigned char CharacterTable_newRow(struct CharacterTable *table, struct CharacterRecord *record)
{
    unsigned short idx = findNextRowToFill(table->rows, MAX_ROWS_CS);
    memcpy(&table->rows[idx], record, sizeof(struct CharacterRecord));
    return table->rows[idx].id;
}

/**
 * @brief Update a row in the table. If the row does not exist, a new row will be created.
 * 
 * @param table  The table to update
 * @param record The record to update
 * @param id     The ID of the row to update
 * @return unsigned char The ID of the updated row
 */
unsigned char CharacterTable_update(struct CharacterTable *table, struct CharacterRecord *record, int id)
{
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            memcpy(&table->rows[i], record, sizeof(struct CharacterRecord));
            return id;
        }
    }
    return CharacterTable_newRow(table, record);
}

/**
 * @brief Get a CharacterRecord from the table by ID.
 * 
 * @param table The table to search
 * @param id    The ID of the character to get
 * @return struct CharacterRecord* 
 */
struct CharacterRecord *CharacterTable_get(struct CharacterTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

/**
 * @brief Get a CharacterRecord from the table by name.
 * 
 * @param table The table to search
 * @param name  The name of the character to get
 * @return struct CharacterRecord* 
 */
struct CharacterRecord *CharacterTable_getByName(struct CharacterTable *table, char *name) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (strcmp(table->rows[i].name, name) == 0) {
            return &table->rows[i];
        }
    }
    return NULL;
}

/**
 * @brief Delete a character from the table by ID.
 * 
 * @param table The table to delete from
 * @param id    The ID of the character to delete
 */
void CharacterTable_delete(struct CharacterTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

/**
 * @brief Print the entire table to the console.
 * 
 * @param table The table to print
 */
void CharacterTable_print(struct CharacterTable *table) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        struct CharacterRecord *row = &table->rows[i];
        if (row->set == 1) {
            CharacterRecord_print(row);
        }
        printf("\n");
    }
}

/**
 * @brief The dedicated destructor for the CharacterTable struct.
 * 
 * @param table 
 */
void CharacterTable_destroy(struct CharacterTable *table) {
    free(table);
}
