#include "location.h"

#include <lcthw/dbg.h>

struct Location *Location_create(char *name, char *description)
{
    struct Location *location = (struct Location *)calloc(1, sizeof(struct Location));

    location->id = 0;
    location->name = strdup(name);
    location->description = strdup(description);
    
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        location->items[i] = NULL;
    }

    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        location->characters[i] = NULL;
    }

    return location;
}

enum MorkResult Location_destroy(struct Location *location)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    free(location);
    return MORK_OK;
}

enum MorkResult Location_save(struct Database *db, struct Location *location)
{
    if (db == NULL) {
        return MORK_ERROR_DB_NULL;
    }
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }

    // First check if description exists
    struct DescriptionRecord *description = Database_getDescriptionByPrefix(db, location->description);
    if (description == NULL) {
        // Create
        struct DescriptionRecord *record = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), location->description, 0);
        record->id = Database_getNextIndex(db, DESCRIPTION);
        enum MorkResult res = Database_createDescription(db, record);
        description = Database_getDescription(db, record->id);
        free(record);
        if (res != MORK_OK) {
            log_err("Failed to save description record");
            return MORK_ERROR_DB_FAILED_SAVE;
        }
    }

    if (location->id == 0) {
        // Location has never been saved before
        // Create a new record for it
        struct LocationRecord *record = LocationRecord_create(
            Database_getNextIndex(db, LOCATIONS),
            location->name,
            description->id
        );
        enum MorkResult res = Database_createLocation(db, record);
        free(record);
        return res;
    } else {
        // Location exists in DB, update it
        struct LocationRecord *record = Database_getLocation(db, location->id);
        if (record == NULL) {
            log_err("Failed to load location record");
            return MORK_ERROR_DB_FAILED_LOAD;
        }

        strncpy(record->name, location->name, MAX_NAME - 1);
        record->descriptionID = description->id;
        return Database_updateLocation(db, record);
    }
}
struct Location *Location_load(struct Database *db, int id)
{
    struct LocationRecord *record = Database_getLocation(db, id);
    if (record == NULL) {
        return NULL;
    }

    struct DescriptionRecord *description = Database_getDescription(db, record->descriptionID);
    if (description == NULL) {
        return NULL;
    }

    struct Location *location = Location_create(record->name, description->description);
    location->id = record->id;

    for (int i = 0; i < MAX_EXITS; i++) {
        location->exitIDs[i] = record->exitIDs[i];
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        location->items[i] = Item_load(db, record->itemIDs[i]);
    }
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        location->characters[i] = Character_loadFromID(db, record->characterIDs[i]);
    }

    return location;
}

enum MorkResult Location_setName(struct Location *location, char *name)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    location->name = name;
    return MORK_OK;
}

enum MorkResult Location_setDescription(struct Location *location, char *description)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    location->description = description;
    return MORK_OK;
}

enum MorkResult Location_addExit(struct Location *location, enum ExitDirection direction, int locationID)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    location->exitIDs[direction] = locationID;
    return MORK_OK;
}

enum MorkResult Location_addItem(struct Location *location, struct Item *item) {
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (location->items[i] == NULL) {
            location->items[i] = item;
            return MORK_OK;
        }
    }
    return MORK_ERROR_MODEL_LOCATION_FULL;
}

enum MorkResult Location_addCharacter(struct Location *location, struct Character *character) 
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (location->characters[i] == NULL) {
            location->characters[i] = character;
            return MORK_OK;
        }
    }
    return MORK_ERROR_MODEL_LOCATION_FULL;
}