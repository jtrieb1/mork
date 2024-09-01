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

#include <lcthw/dbg.h>

static unsigned short findNextRowToFill(struct CharacterTable *table)
{
    unsigned short idx = 0;
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == 0) {
            idx = i;
            break;
        }
    }
    if (idx == 0) {
        // No empty rows, so find oldest and overwrite
        int min_id = 65535;
        for (int i = 0; i < MAX_ROWS_CS; i++) {
            if (table->rows[i].id < min_id) {
                min_id = table->rows[i].id;
                idx = i;
            }
        }
    }
    return idx;
}

struct CharacterRecord *CharacterRecord_default() {
    return CharacterRecord_create(
        "", 0, 0, 0, 0, 0, 0, 0
    );
}

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

void CharacterRecord_destroy(struct CharacterRecord *record) {
    free(record);
}

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

struct CharacterTable *CharacterTable_create() {
    struct CharacterTable *table = (struct CharacterTable *)calloc(1, sizeof(struct CharacterTable));
    check_mem(table);
    CharacterTable_init(table);
    return table;

error:
    return NULL;
}

void CharacterTable_init(struct CharacterTable *table) {
    struct CharacterRecord *record = CharacterRecord_default();
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        memcpy(&table->rows[i], record, sizeof(struct CharacterRecord));
    }
    free(record);
}

unsigned char CharacterTable_newRow(struct CharacterTable *table, struct CharacterRecord *record)
{
    unsigned short idx = findNextRowToFill(table);
    memcpy(&table->rows[idx], record, sizeof(struct CharacterRecord));
    return table->rows[idx].id;
}

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

struct CharacterRecord *CharacterTable_get(struct CharacterTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

struct CharacterRecord *CharacterTable_getByName(struct CharacterTable *table, char *name) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (strcmp(table->rows[i].name, name) == 0) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void CharacterTable_delete(struct CharacterTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

void CharacterTable_print(struct CharacterTable *table) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        struct CharacterRecord *row = &table->rows[i];
        if (row->set == 1) {
            CharacterRecord_print(row);
        }
        printf("\n");
    }
}

void CharacterTable_destroy(struct CharacterTable *table) {
    free(table);
}
