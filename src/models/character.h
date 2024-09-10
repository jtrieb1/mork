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

#include "../coredb/db.h"
#include "inventory.h"

struct Character {
    unsigned short id;
    char name[MAX_NAME_LEN];
    unsigned char level;
    unsigned long experience;
    unsigned short health;
    unsigned short mana;
    unsigned short max_health;
    unsigned short max_mana;
    unsigned char numStats;
    unsigned char stats[16];

    struct Inventory *inventory;
};

struct Character *Character_create(
    char *name,
    unsigned char level,
    unsigned char *stats,
    unsigned char numStats
);
void Character_destroy(struct Character *character);

struct Character *Character_clone(struct Character *source);

int Character_save(struct Database *db, struct Character *character);
struct Character *Character_load(struct Database *db, char *name);
struct Character *Character_loadFromID(struct Database *db, unsigned char id);

unsigned short Character_getStat(struct Character *character, unsigned char stat);
