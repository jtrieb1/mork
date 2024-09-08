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

unsigned short calculateMaxHealth(unsigned char level)
{
    return 100 + (level * 10);
}

unsigned short calculateMaxMana(unsigned char level)
{
    return 50 + (level * 10);
}

struct Character *Character_create(
    char *name,
    unsigned char level,
    unsigned char *stats,
    unsigned char numStats
)
{
    struct Character *character = (struct Character *)calloc(1, sizeof(struct Character));
    check_mem(character);

    strncpy(character->name, name, MAX_NAME_LEN);
    check_mem(character->name);
    character->name[MAX_NAME_LEN - 1] = '\0';

    character->level = level;
    character->experience = 0;
    character->health = calculateMaxHealth(level);
    character->max_health = calculateMaxHealth(level);
    character->mana = calculateMaxMana(level);
    character->max_mana = calculateMaxMana(level);

    character->numStats = numStats;
    memcpy(character->stats, stats, numStats);

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
        source->level,
        source->stats,
        source->numStats
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

int Character_save(struct Database *db, struct Character *character)
{
    int character_id = -1;
    struct CharacterRecord *existing = Database_getCharacterByName(db, character->name);
    if (existing == NULL) {
        // Create
        struct CharacterRecord *record = CharacterRecord_create(
            character->name,
            character->level,
            character->health,
            character->max_health,
            character->mana,
            character->max_mana,
            character->stats,
            character->numStats
        );
        record->id = Database_getNextIndex(db, CHARACTERS);
        enum MorkResult res = Database_createCharacter(db, record);
        character_id = record->id;
        free(record);
        if (res != MORK_OK) {
            log_err("Failed to save character record");
            return -1;
        }
    } else {
        // Update
        existing->level = character->level;
        existing->experience = character->experience;
        existing->health_and_mana = character->health;
        existing->max_health_and_mana = character->max_health;
        existing->numStats = character->numStats;

        for (int i = 0; i < character->numStats; i++) {
            existing->stats = SET_STAT(existing->stats, i, character->stats[i]);
        }

        enum MorkResult res = Database_updateCharacter(db, existing);
        if (res != MORK_OK) {
            log_err("Failed to update character record");
            return -1;
        }
        character_id = existing->id;
    }

    // Save inventory
    check(Inventory_save(db, character_id, character->inventory) != -1, "Failed to save inventory");\
    return character_id;

error:

    return -1;
}

struct Character *Character_load(struct Database *db, char *name)
{
    struct CharacterRecord *record = Database_getCharacterByName(db, name);
    check(record != NULL, "Failed to load character record");

    unsigned char *stats = (unsigned char *)calloc(record->numStats, sizeof(unsigned char));
    for (int i = 0; i < record->numStats; i++) {
        stats[i] = GET_STAT(record->stats, i);
    }

    struct Character *character = Character_create(
        record->name,
        record->level,
        stats,
        record->numStats
    );
    check(character, "Failed to create character");

    character->level = record->level;
    character->experience = record->experience;
    character->health = GET_HEALTH(record->health_and_mana);
    character->max_health = GET_HEALTH(record->max_health_and_mana);
    character->mana = GET_MANA(record->health_and_mana);
    character->max_mana = GET_MANA(record->max_health_and_mana);
    memcpy(character->stats, stats, record->numStats);
    free(stats);

    // They get an inventory during creation, so destroy it
    Inventory_destroy(character->inventory);
    character->inventory = Inventory_load(db, record->id);

    return character;

error:
    return NULL;
}

unsigned short Character_getStat(struct Character *character, unsigned char stat) {
    return character->stats[stat];
}
