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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NAME_LEN 232 // Cap names at 231 characters, explained below
#define MAX_ROWS_CS 256  // Maximum number of rows in the table

// We can limit the data we need to store by imposing certain restrictions.
// In Mork, we'll say that each stat is at most 16, so that each stat can be
// stored in 4 bits. This means that we can store all of the stats in a single
// 64-bit integer. We'll use the following bit layout:
// 0-3: Strength
// 4-7: Dexterity
// 8-11: Intelligence
// 12-15: Charisma
// 16-19: Wisdom
// 20-23: Funkiness
//
// Of course, this makes retrieving this information slightly more difficult
// than if we stored each stat in its own field, but it's a tradeoff we're
// willing to make for the sake of saving space. We'll write some helper
// functions to make it easier to work with these stats.

// We'll also say that the maximum level is 255, and the maximum health and
// mana are 65535. This means that we can store the level in 8 bits, and the
// health and mana in 16 bits, which we'll combine. We'll store the experience
// in 32 bits, which means that we can store up to 4,294,967,295 experience points.

// The total size so far is 2 + 1 + 1 + 4 + 4 + 8 = 20 bytes.
// It's usually a good idea to align data structures to a power of 2, so we keep that
// in mind when adding the name field. We'll say that the maximum name length is 231
// characters, which means that we need 232 bytes to store the name, since we need to
// store the null terminator. This means that the total size of each row is 256 bytes.

struct CharacterStatsRecord {
    unsigned short id;                  // 2 bytes
    unsigned char set;                  // 1 byte
    unsigned char level;                // 1 byte
    unsigned long experience;           // 4 bytes
    unsigned long health_and_mana;      // 4 bytes
    unsigned long max_health_and_mana;  // 4 bytes
    unsigned long long stats;           // 8 bytes
    char name[MAX_NAME_LEN];            // 232 bytes
                                        // Total: 256 bytes per row
};

// Helper functions to work with the stats field
// We'll use the following macros to extract and set the stats from the stats field
#define GET_STAT(stats, stat) ((stats >> (4 * stat)) & 0xF)
#define SET_STAT(stats, stat, value) (stats = (stats & ~(0xF << (4 * stat))) | ((value & 0xF) << (4 * stat)))
// To use those macros, we'll need the following enum to represent the stats
enum Stats {
    STRENGTH = 0,
    DEXTERITY = 1,
    INTELLIGENCE = 2,
    CHARISMA = 3,
    WISDOM = 4,
    FUNKINESS = 5
};

// Helper functions to work with the health_and_mana field
// We'll use the following macros to extract and set the health and mana from the health_and_mana field
// Luckily, we can reuse these for max_health_and_mana as well
#define GET_HEALTH(health_and_mana) (health_and_mana & 0xFFFF)
#define GET_MANA(health_and_mana) ((health_and_mana >> 16) & 0xFFFF)
#define SET_HEALTH_AND_MANA(health, mana) ((health & 0xFFFF) | ((mana & 0xFFFF) << 16))

struct CharacterStatsRecord *CharacterStatsRecord_default();
void CharacterStatsRecord_init(struct CharacterStatsRecord *rec);
struct CharacterStatsRecord *CharacterStatsRecord_create(
    int id, int set,
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
);
void CharacterStatsRecord_destroy(struct CharacterStatsRecord *rec);
void CharacterStatsRecord_print(struct CharacterStatsRecord *rec);

// We know that the maximum number of rows is 256,
// so we can use unsigned chars to store row index data
struct CharacterStatsTable {
    unsigned char nextEmptyRow;
    unsigned char maxOccupiedRow;
    struct CharacterStatsRecord rows[MAX_ROWS_CS];
};

struct CharacterStatsTable *CharacterStatsTable_create();

void CharacterStatsTable_init(struct CharacterStatsTable *table);
void CharacterStatsTable_set(
    struct CharacterStatsTable *table,
    struct CharacterStatsRecord *record
);
struct CharacterStatsRecord *CharacterStatsTable_get(struct CharacterStatsTable *table, int id);
void CharacterStatsTable_delete(struct CharacterStatsTable *table, int id);

void CharacterStatsTable_destroy(struct CharacterStatsTable *table);
void CharacterStatsTable_print(struct CharacterStatsTable *table);
