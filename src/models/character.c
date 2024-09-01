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
    check(character->inventory, "Failed to create inventory");

    return character;

error:
    return NULL;
}

struct Character *Character_clone(struct Character *source)
{
    struct Character *character = Character_create(
        source->name,
        source->strength,
        source->dexterity,
        source->intelligence,
        source->wisdom,
        source->charisma,
        source->funkiness
    );
    check(character, "Failed to clone character");

    character->level = source->level;
    character->experience = source->experience;
    character->health = source->health;
    character->max_health = source->max_health;
    character->mana = source->mana;
    character->max_mana = source->max_mana;

    // The clone inherits the source's inventory
    character->inventory = Inventory_clone(source->inventory);

    return character;

error:
    return NULL;
}

void Character_destroy(struct Character *character)
{
    if (character) {
        if (character->inventory) {
            Inventory_destroy(character->inventory);
        }
        free(character);
    }

    character = NULL;
}

void Character_save(struct Database *db, struct Character *character)
{
    int character_id = -1;
    struct CharacterRecord *existing = Database_getCharacterStatsByName(db, character->name);
    if (existing == NULL) {
        // Create
        struct CharacterRecord *record = CharacterRecord_create(
            character->name,
            character->level,
            character->health,
            character->max_health,
            character->mana,
            character->max_mana,
            (unsigned char[6]){
                character->strength,
                character->dexterity,
                character->intelligence,
                character->wisdom,
                character->charisma,
                character->funkiness
            },
            6
        );
        character_id = Database_createCharacterStats(db, record);
        free(record);
    } else {
        // Update
        existing->level = character->level;
        existing->experience = character->experience;
        existing->health_and_mana = character->health;
        existing->max_health_and_mana = character->max_health;
        existing->stats = SET_STAT(existing->stats, STRENGTH, character->strength);
        existing->stats = SET_STAT(existing->stats, DEXTERITY, character->dexterity);
        existing->stats = SET_STAT(existing->stats, INTELLIGENCE, character->intelligence);
        existing->stats = SET_STAT(existing->stats, WISDOM, character->wisdom);
        existing->stats = SET_STAT(existing->stats, CHARISMA, character->charisma);
        existing->stats = SET_STAT(existing->stats, FUNKINESS, character->funkiness);

        character_id = Database_updateCharacterStats(db, existing, existing->id);
    }

    // Save inventory
    check(Inventory_save(db, character_id, character->inventory) != -1, "Failed to save inventory");\
    return;

error:

    return;
}

struct Character *Character_load(struct Database *db, char *name)
{
    struct CharacterRecord *record = Database_getCharacterStatsByName(db, name);
    check(record != NULL, "Failed to load character record");

    struct Character *character = Character_create(
        record->name,
        GET_STAT(record->stats, STRENGTH),
        GET_STAT(record->stats, DEXTERITY),
        GET_STAT(record->stats, INTELLIGENCE),
        GET_STAT(record->stats, WISDOM),
        GET_STAT(record->stats, CHARISMA),
        GET_STAT(record->stats, FUNKINESS)
    );
    check(character, "Failed to create character");

    character->level = record->level;
    character->experience = record->experience;
    character->health = record->health_and_mana;
    character->max_health = record->max_health_and_mana;
    character->mana = record->health_and_mana;
    character->max_mana = record->max_health_and_mana;

    // They get an inventory during creation, so destroy it
    Inventory_destroy(character->inventory);
    character->inventory = Inventory_load(db, record->id);

    return character;

error:
    return NULL;
}
