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
    character->id = 0;

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
    if (character == NULL) {
        log_err("Character is NULL");
        return -1;
    }
    if (strcmp(character->name, "") == 0) {
        log_err("Character name is empty");
        return -1;
    }
    if (character->id == 0) {
        // Character has never been saved before
        // Create a new record for them
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
        if (res != MORK_OK) {
            log_err("Failed to save character record");
            return -1;
        }
        character->id = record->id;
    } else {
        // Character has been saved before
        // Get them from the database
        struct CharacterRecord *existing = Database_getCharacter(db, character->id);
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
    }

    // Save inventory
    check(Inventory_save(db, character->id, character->inventory) != -1, "Failed to save inventory");\
    return character->id;

error:

    return -1;
}

struct Character *Character_fromRecord(struct Database *db, struct CharacterRecord rec)
{
    unsigned char *stats = (unsigned char *)calloc(rec.numStats, sizeof(unsigned char));
    for (int i = 0; i < rec.numStats; i++) {
        stats[i] = GET_STAT(rec.stats, i);
    }

    struct Character *character = Character_create(
        rec.name,
        rec.level,
        stats,
        rec.numStats
    );
    check(character, "Failed to create character");

    character->level = rec.level;
    character->experience = rec.experience;
    character->health = GET_HEALTH(rec.health_and_mana);
    character->max_health = GET_HEALTH(rec.max_health_and_mana);
    character->mana = GET_MANA(rec.health_and_mana);
    character->max_mana = GET_MANA(rec.max_health_and_mana);
    memcpy(character->stats, stats, rec.numStats);
    free(stats);

    // Destroy the dummy inventory that was created during character creation
    Inventory_destroy(character->inventory);
    character->inventory = Inventory_load(db, rec.id);

    return character;

error:
    return NULL;
}

struct Character *Character_load(struct Database *db, char *name)
{
    struct CharacterRecord *record = Database_getCharacterByName(db, name);
    check(record != NULL, "Failed to load character record");

    return Character_fromRecord(db, *record);

error:
    return NULL;
}

struct Character *Character_loadFromID(struct Database *db, unsigned char id)
{
    struct CharacterRecord *record = Database_getCharacter(db, id);
    check(record != NULL, "Failed to load character record");

    return Character_fromRecord(db, *record);

error:
    return NULL;
}

unsigned short Character_getStat(struct Character *character, unsigned char stat) {
    return character->stats[stat];
}
