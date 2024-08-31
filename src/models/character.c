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
#include "inventory.h"

#include <lcthw/dbg.h>

struct Character *Character_create(const char *name, unsigned short strength, unsigned short dexterity, unsigned short intelligence, unsigned short wisdom, unsigned short charisma, unsigned short funkiness)
{
    struct Character *character = (struct Character *)malloc(sizeof(struct Character));
    check_mem(character);

    strncpy(character->name, name, MAX_NAME_LEN);
    check_mem(character->name);
    character->name[MAX_NAME_LEN - 1] = '\0';

    // New characters start at level 1
    character->level = 1;
    character->experience = 0;
    character->health = 100;
    character->max_health = 100;
    character->mana = 100;
    character->max_mana = 100;

    character->strength = strength;
    character->dexterity = dexterity;
    character->intelligence = intelligence;
    character->wisdom = wisdom;
    character->charisma = charisma;
    character->funkiness = funkiness;

    // Character starts with empty inventory
    character->inventory = Inventory_create();

    return character;

error:
    return NULL;
}

void Character_destroy(struct Character *character)
{
    if (character) {
        Inventory_destroy(character->inventory);
        free(character);
    }
}

void Character_save(struct Database *db, struct Character *character)
{
    struct CharacterStatsRecord *record = CharacterStatsRecord_create(
        character->name,
        character->level,
        character->experience,
        character->health,
        character->max_health,
        character->mana,
        character->max_mana,
        character->strength,
        character->dexterity,
        character->intelligence,
        character->wisdom,
        character->charisma,
        character->funkiness
    );
    check(record, "Failed to create character record");

    Database_set_character_stats(db, record);

    // Save inventory
    Inventory_save(db, character->name,  character->inventory);

    return;

error:

    return;
}
