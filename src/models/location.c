#include "location.h"

#include <lcthw/dbg.h>

enum ExitDirection oppositeDirection(enum ExitDirection direction)
{
    switch (direction)
    {
    case NORTH:
        return SOUTH;
    case SOUTH:
        return NORTH;
    case EAST:
        return WEST;
    case WEST:
        return EAST;
    case UP:
        return DOWN;
    case DOWN:
        return UP;
    default:
        return -1;
    }
}

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
    free(location->name);
    free(location->description);
    free(location);
    return MORK_OK;
}

struct LocationRecord *Location_asRecord(struct Database *db, struct Location *location)
{
    if (location == NULL) {
        return NULL;
    }

    // Get description record
    struct DescriptionRecord *description = Database_getDescriptionByPrefix(db, location->description);
    if (description == NULL) {
        // Create it
        description = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), location->description, 0);
        // Make sure to add to DB
        enum MorkResult res = Database_createDescription(db, description);
        if (res != MORK_OK) {
            log_err("Failed to create description record.");
            return NULL;
        }

        // Reload it
        free(description);
        description = Database_getDescriptionByPrefix(db, location->description);
        if (description == NULL) {
            log_err("Failed to reload description record.");
            return NULL;
        }
    }

    struct LocationRecord *record = LocationRecord_create(
        location->id,
        location->name,
        description->id
    );

    for (int i = 0; i < MAX_EXITS; i++) {
        record->exitIDs[i] = location->exitIDs[i];
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (location->items[i] != NULL) {
            record->itemIDs[i] = location->items[i]->id;
        }
    }

    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (location->characters[i] != NULL) {
            record->characterIDs[i] = location->characters[i]->id;
        }
    }

    return record;
}

enum MorkResult Location_save(struct Database *db, struct Location *location)
{
    if (db == NULL) {
        return MORK_ERROR_DB_NULL;
    }
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }

    struct LocationRecord *updated = Location_asRecord(db, location);
    if (updated == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }

    if (updated->id == 0) {
        // Record has never been saved
        // Set ID to next available ID
        updated->id = Database_getNextIndex(db, LOCATIONS);
        enum MorkResult res = Database_createLocation(db, updated);
        free(updated);
        return res;
    } else {
        // Record exists in DB, update it
        enum MorkResult res = Database_updateLocation(db, updated);
        free(updated);
        return res;
    }
}

struct Location *Location_load(struct Database *db, int id)
{
    struct LocationRecord *record = Database_getLocation(db, id);
    if (record == NULL) {
        log_err("Failed to load location record.");
        return NULL;
    }

    struct DescriptionRecord *description = Database_getDescription(db, record->descriptionID);
    if (description == NULL) {
        log_err("Failed to load description record.");
        return NULL;
    }

    struct Location *location = Location_create(record->name, description->description);
    if (location == NULL) {
        log_err("Failed to create location.");
        return NULL;
    }
    location->id = record->id;

    for (int i = 0; i < MAX_EXITS; i++) {
        location->exitIDs[i] = record->exitIDs[i];
    }

    // Load items
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (record->itemIDs[i] != 0) {
            struct Item *item = Item_load(db, record->itemIDs[i]);
            if (item == NULL) {
                log_err("Failed to load item.");
                return NULL;
            }
            location->items[i] = item;
        }
    }

    // Load characters
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (record->characterIDs[i] != 0) {
            struct Character *character = Character_loadFromID(db, record->characterIDs[i]);
            if (character == NULL) {
                log_err("Failed to load character.");
                return NULL;
            }
            location->characters[i] = character;
        }
    }

    return location;
}

enum MorkResult Location_setName(struct Location *location, char *name)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    strncpy(location->name, name, MAX_NAME - 1);
    return MORK_OK;
}

enum MorkResult Location_setDescription(struct Location *location, char *description)
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    strncpy(location->description, description, MAX_DESCRIPTION - 1);
    return MORK_OK;
}

enum MorkResult Location_addExit(struct Location *from, enum ExitDirection direction, struct Location *to)
{
    if (from == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    from->exitIDs[direction] = to->id;
    to->exitIDs[oppositeDirection(direction)] = from->id;

    return MORK_OK;
}

enum MorkResult Location_removeExit(struct Location *from, enum ExitDirection direction)
{
    if (from == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    from->exitIDs[direction] = 0;
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

enum MorkResult Location_removeItem(struct Location *location, struct Item *item) {
    // Find and destroy item
    log_info("Item info: %d, %s", item->id, item->name);
    if (location == NULL) {
        log_info("Uh oh, location is NULL");
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        log_info("Checking index: %d", i);
        if (location->items[i] == NULL) {
            log_info("Item is NULL");
            continue;
        }
        log_info("Location item ID: %d", location->items[i]->id);
        if (location->items[i]->id == item->id) {
            Item_destroy(location->items[i]);
            location->items[i] = NULL;
            return MORK_OK;
        }
    }
    return MORK_ERROR_MODEL_ITEM_NOT_FOUND;
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

enum MorkResult Location_removeCharacter(struct Location *location, struct Character *character) 
{
    if (location == NULL) {
        return MORK_ERROR_MODEL_LOCATION_NULL;
    }
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (location->characters[i] == character) {
            location->characters[i] = NULL;
            return MORK_OK;
        }
    }
    return MORK_ERROR_MODEL_CHARACTER_NOT_FOUND;
}

int Location_getItemCount(struct Location *location) 
{
    if (location == NULL) {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (location->items[i] != NULL && location->items[i]->id != 0) {
            count++;
        }
    }
    return count;
}

struct Location *Location_loadByName(struct Database *db, char *name)
{
    struct LocationRecord *record = Database_getLocationByName(db, name);
    
    if (record == NULL) {
        log_err("Failed to load location record.");
        return NULL;
    }
    return Location_load(db, record->id);
}