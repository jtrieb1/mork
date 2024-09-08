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

#include "../../utils/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NAME_LEN 231 // Cap names at 230 characters
#define MAX_ROWS_CS 256  // Maximum number of rows in the table

// Since we're turning this into a library, we need to be more generic with what
// we do.

// We'll define a struct to represent a single row in the table.

struct CharacterRecord {
    unsigned short id;                  // 2 bytes
    unsigned char set;                  // 1 byte
    unsigned char level;                // 1 byte
    unsigned long experience;           // 4 bytes
    unsigned long health_and_mana;      // 4 bytes
    unsigned long max_health_and_mana;  // 4 bytes
    unsigned long long stats;           // 8 bytes
    unsigned char numStats;             // 1 byte
    char name[MAX_NAME_LEN];            // 231 bytes
                                        // Total: 256 bytes per row
};

// Helper functions to work with the stats field
// We'll use the following macros to extract and set the stats from the stats field
#define GET_STAT(stats, stat) ((stats >> (4 * stat)) & 0xF)
#define SET_STAT(stats, stat, value) (stats = (stats & ~(0xF << (4 * stat))) | ((value & 0xF) << (4 * stat)))

// IMPORTANT: Those macros will work with any enum that has 16 or fewer values
// Define custom stats for your game

// Helper functions to work with the health_and_mana field
// We'll use the following macros to extract and set the health and mana from the health_and_mana field
// Luckily, we can reuse these for max_health_and_mana as well
#define GET_HEALTH(health_and_mana) (health_and_mana & 0xFFFF)
#define GET_MANA(health_and_mana) ((health_and_mana >> 16) & 0xFFFF)
#define SET_HEALTH_AND_MANA(health, mana) ((health & 0xFFFF) | ((mana & 0xFFFF) << 16))

struct CharacterRecord *CharacterRecord_create(
    char *name,
    unsigned int level,
    unsigned int health,
    unsigned int maxHealth,
    unsigned int mana,
    unsigned int maxMana,
    unsigned char *stats,
    unsigned int numStats
);
enum MorkResult CharacterRecord_destroy(struct CharacterRecord *rec);
enum MorkResult CharacterRecord_print(struct CharacterRecord *rec);

// We know that the maximum number of rows is 256,
// so we can use unsigned chars to store row index data
struct CharacterTable {
    struct CharacterRecord rows[MAX_ROWS_CS];
};

struct CharacterTable *CharacterTable_create();

enum MorkResult CharacterTable_init(struct CharacterTable *table);
enum MorkResult CharacterTable_newRow(struct CharacterTable *table, struct CharacterRecord *record);
enum MorkResult CharacterTable_update(struct CharacterTable *table, struct CharacterRecord *record);

struct CharacterRecord *CharacterTable_get(struct CharacterTable *table, int id);
struct CharacterRecord *CharacterTable_getByName(struct CharacterTable *table, char *name);
enum MorkResult CharacterTable_delete(struct CharacterTable *table, int id);

enum MorkResult CharacterTable_destroy(struct CharacterTable *table);
enum MorkResult CharacterTable_print(struct CharacterTable *table);
