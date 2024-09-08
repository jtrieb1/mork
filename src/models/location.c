#include "location.h"

#include <lcthw/dbg.h>

struct Location *Location_create(char *name, char *description)
{
    struct Location *location = (struct Location *)calloc(1, sizeof(struct Location));
    location->name = name;
    location->description = description;

    memset(location->exitIDs, MAX_EXITS, sizeof(int));
    memset(location->items, MAX_ITEMS, sizeof(struct Item));
    memset(location->characters, MAX_CHARACTERS, sizeof(struct Character));
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
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }

    struct LocationRecord *record = Database_getLocationByName(db, location->name);
    if (record == NULL) {
        int nextID = Database_getNextIndex(db, LOCATIONS);

        // Find description record
        struct DescriptionRecord *descriptionRecord = Database_getDescriptionByPrefix(db, location->description);

        record = LocationRecord_create(nextID, location->name, descriptionRecord == NULL ? 0 : descriptionRecord->id);
        check(record != NULL, "Failed to create location record.");

        enum MorkResult res = Database_createLocation(db, record);
        free(record);
        if (res != MORK_OK) {
            log_err("Failed to create location record.");

            goto error;
        }
        return MORK_OK;
    } else {
        // Location exists in DB, update it.
        return Database_updateLocation(db, record);
    }

error:
    return MORK_ERROR_DB_FAILED_SAVE;
}
struct Location *Location_load(struct Database *db, int id)
{
    struct LocationRecord *record = Database_getLocation(db, id);
    check(record != NULL, "Failed to load location.");

    // Get description record
    struct DescriptionRecord *descriptionRecord = Database_getDescription(db, record->descriptionID);

    struct Location *location = Location_create(record->name, descriptionRecord->description);
    
    for (int i = 0; i < MAX_EXITS; i++)
    {
        location->exitIDs[i] = record->exitIDs[i];
    }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        struct Item *item = Item_load(db, record->itemIDs[i]);
        if (item != NULL)
        {
            location->items[i] = *item;
        }
    }

    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        struct Character *character = Character_loadFromID(db, record->characterIDs[i]);
        if (character != NULL)
        {
            location->characters[i] = *character;
        }
    }

    return location;

error:
    return NULL;
}