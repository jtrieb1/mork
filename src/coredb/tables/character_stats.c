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

#include "character_stats.h"

#include <lcthw/dbg.h>

struct CharacterStatsRecord *CharacterStatsRecord_default() {
    struct CharacterStatsRecord *record = (struct CharacterStatsRecord *)malloc(sizeof(struct CharacterStatsRecord));
    check_mem(record);
    CharacterStatsRecord_init(record);
    return record;

error:
    return NULL;
}

void CharacterStatsRecord_init(struct CharacterStatsRecord *record) {
    record->id = 0;
    record->set = 0;
    record->name[0] = '\0';
    record->level = 0;
    record->experience = 0;
    record->health_and_mana = 0;
    record->max_health_and_mana = 0;
    record->stats = 0;
}

struct CharacterStatsRecord *CharacterStatsRecord_create(
    const char *name,
    unsigned int level,
    unsigned int experience,
    unsigned int health,
    unsigned int maxHealth,
    unsigned int mana,
    unsigned int maxMana,
    unsigned int strength,
    unsigned int dexterity,
    unsigned int intelligence,
    unsigned int charisma,
    unsigned int wisdom,
    unsigned int funkiness
) {
    struct CharacterStatsRecord *record = (struct CharacterStatsRecord *)malloc(sizeof(struct CharacterStatsRecord));
    check_mem(record);
    CharacterStatsRecord_init(record);
    record->id = 0;
    record->set = 0;
    strncpy(record->name, name, MAX_NAME_LEN - 1);
    record->level = level;
    record->experience = experience;
    record->health_and_mana = SET_HEALTH_AND_MANA(health, mana);
    record->max_health_and_mana = SET_HEALTH_AND_MANA(maxHealth, maxMana);
    record->stats = SET_STAT(record->stats, STRENGTH, strength);
    record->stats = SET_STAT(record->stats, DEXTERITY, dexterity);
    record->stats = SET_STAT(record->stats, INTELLIGENCE, intelligence);
    record->stats = SET_STAT(record->stats, CHARISMA, charisma);
    record->stats = SET_STAT(record->stats, WISDOM, wisdom);
    record->stats = SET_STAT(record->stats, FUNKINESS, funkiness);

    return record;

error:
    return NULL;
}

void CharacterStatsRecord_destroy(struct CharacterStatsRecord *record) {
    free(record);
}

void CharacterStatsRecord_print(struct CharacterStatsRecord *record) {
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
    printf("Strength: %llu\n", GET_STAT(record->stats, STRENGTH));
    printf("Dexterity: %llu\n", GET_STAT(record->stats, DEXTERITY));
    printf("Intelligence: %llu\n", GET_STAT(record->stats, INTELLIGENCE));
    printf("Charisma: %llu\n", GET_STAT(record->stats, CHARISMA));
    printf("Wisdom: %llu\n", GET_STAT(record->stats, WISDOM));
    printf("Funkiness: %llu\n", GET_STAT(record->stats, FUNKINESS));
}

struct CharacterStatsTable *CharacterStatsTable_create() {
    struct CharacterStatsTable *table = (struct CharacterStatsTable *)malloc(sizeof(struct CharacterStatsTable));
    check_mem(table);
    CharacterStatsTable_init(table);
    return table;

error:
    return NULL;
}

void CharacterStatsTable_init(struct CharacterStatsTable *table) {
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        table->rows[i].id = 0;
        table->rows[i].set = 0;
    }
}

unsigned char CharacterStatsTable_set(
    struct CharacterStatsTable *table,
    struct CharacterStatsRecord *new_row
) {
    // Check for existing row with matching id
    for (unsigned char i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == new_row->id) {
            table->rows[i] = *new_row;
            return i;
        }
    }

    // If no existing row with matching id, add new row
    if (table->maxOccupiedRow < MAX_ROWS_CS - 1) {
        // Table is not full here
        // Find where the row needs to go
        // Is nextEmptyRow empty?
        if (table->rows[table->nextEmptyRow].set == 0) {
            table->rows[table->nextEmptyRow] = *new_row;
            table->maxOccupiedRow = table->nextEmptyRow;
            table->nextEmptyRow++;
            return table->maxOccupiedRow;
        } else {
            // Find the next empty row
            for (unsigned char i = 0; i < MAX_ROWS_CS - 1; i++) {
                if (table->rows[i].set == 0) {
                    table->rows[i] = *new_row;
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
        for (unsigned char i = 0; i < MAX_ROWS_CS - 1; i++) {
            if (table->rows[i].id < table->rows[oldestRow].id) {
                oldestRow = i;
            }
        }
        table->rows[oldestRow] = *new_row;
        return oldestRow;
    }
}

struct CharacterStatsRecord *CharacterStatsTable_get(struct CharacterStatsTable *table, int id) {
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

struct CharacterStatsRecord *CharacterStatsTable_get_by_name(struct CharacterStatsTable *table, char *name) {
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (strcmp(table->rows[i].name, name) == 0) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void CharacterStatsTable_delete(struct CharacterStatsTable *table, int id) {
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

void CharacterStatsTable_print(struct CharacterStatsTable *table) {
    for (int i = 0; i < table->maxOccupiedRow; i++) {
        struct CharacterStatsRecord *row = &table->rows[i];
        if (row->set == 1) {
            printf("[CharacterStatsRow] Name=%s, Health=%lu, Mana=%lu Strength=%llu, Dexterity=%llu, Intelligence=%llu, Wisdom=%llu, Charisma=%llu\n",
                row->name,
                GET_HEALTH(row->health_and_mana), GET_MANA(row->health_and_mana),
                GET_STAT(row->stats, STRENGTH), GET_STAT(row->stats, DEXTERITY),
                GET_STAT(row->stats, INTELLIGENCE), GET_STAT(row->stats, WISDOM),
                GET_STAT(row->stats, CHARISMA)
            );
        }
        printf("\n");
    }
}

void CharacterStatsTable_destroy(struct CharacterStatsTable *table) {
    free(table);
}
