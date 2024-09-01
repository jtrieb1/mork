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

static unsigned short findNextRowToFill(struct CharacterStatsTable *table)
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

struct CharacterStatsRecord *CharacterStatsRecord_default() {
    return CharacterStatsRecord_create(
        "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    );
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
    struct CharacterStatsRecord *record = (struct CharacterStatsRecord *)calloc(1, sizeof(struct CharacterStatsRecord));
    check_mem(record);
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
    struct CharacterStatsTable *table = (struct CharacterStatsTable *)calloc(1, sizeof(struct CharacterStatsTable));
    check_mem(table);
    CharacterStatsTable_init(table);
    return table;

error:
    return NULL;
}

void CharacterStatsTable_init(struct CharacterStatsTable *table) {
    struct CharacterStatsRecord *record = CharacterStatsRecord_default();
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        memcpy(&table->rows[i], record, sizeof(struct CharacterStatsRecord));
    }
    free(record);
}

unsigned char CharacterStatsTable_newRow(struct CharacterStatsTable *table, struct CharacterStatsRecord *record)
{
    unsigned short idx = findNextRowToFill(table);
    memcpy(&table->rows[idx], record, sizeof(struct CharacterStatsRecord));
    return table->rows[idx].id;
}

unsigned char CharacterStatsTable_update(struct CharacterStatsTable *table, struct CharacterStatsRecord *record, int id)
{
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            memcpy(&table->rows[i], record, sizeof(struct CharacterStatsRecord));
            return id;
        }
    }
    return CharacterStatsTable_newRow(table, record);
}

struct CharacterStatsRecord *CharacterStatsTable_get(struct CharacterStatsTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

struct CharacterStatsRecord *CharacterStatsTable_getByName(struct CharacterStatsTable *table, char *name) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (strcmp(table->rows[i].name, name) == 0) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void CharacterStatsTable_delete(struct CharacterStatsTable *table, int id) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}

void CharacterStatsTable_print(struct CharacterStatsTable *table) {
    for (int i = 0; i < MAX_ROWS_CS; i++) {
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
