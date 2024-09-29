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

#include "../coredb/db.h"
#include "item.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct Item *Item_create(const char *name, const char *description)
{
    check(name != NULL, "Expected a non-null name.");
    check(description != NULL, "Expected a non-null description.");
    check(strcmp(name, "") != 0, "Expected a non-empty name.");
    check(strcmp(description, "") != 0, "Expected a non-empty description.");

    struct Item *item = malloc(sizeof(struct Item));
    check_mem(item);

    item->id = 0;

    strncpy(item->name, name, MAX_NAME);
    item->name[MAX_NAME - 1] = '\0';

    strncpy(item->description, description, MAX_DESCRIPTION);
    item->description[MAX_DESCRIPTION - 1] = '\0';

    return item;

error:
    return NULL;
}

struct Item *Item_clone(struct Item *source)
{
    check(source != NULL, "Expected a non-null source item.");
    struct Item *pItem = Item_create(source->name, source->description);
    pItem->id = source->id;
    return pItem;

error:
    return NULL;
}

enum MorkResult Item_destroy(struct Item *item)
{
    if (item == NULL) {
        return MORK_ERROR_MODEL_ITEM_NULL;
    }
    free(item);
    return MORK_OK;
}

struct DescriptionRecord *Item_getDescriptionRecord(struct Database *db, struct Item *item)
{
    check(db != NULL, "Database is NULL");
    check(item != NULL, "Item is NULL");

    // See if item description exists in database
    struct DescriptionRecord *descriptionRecord = Database_getDescriptionByPrefix(db, item->description);

    if (descriptionRecord == NULL) {
        // Description does not exist in DB, create it.
        int nextDescriptionID = Database_getNextIndex(db, DESCRIPTION);
        descriptionRecord = DescriptionRecord_create(nextDescriptionID, item->description, 0);
        check(descriptionRecord != NULL, "Failed to create description record.");

        descriptionRecord->id = nextDescriptionID;
        enum MorkResult res = Database_createDescription(db, descriptionRecord);
        if (res != MORK_OK) {
            log_err("Failed to create description record.");
            free(descriptionRecord);
            return NULL;
        }

        free(descriptionRecord);
        descriptionRecord = Database_getDescription(db, nextDescriptionID);
    }

    return descriptionRecord;

error:
    return NULL;
}

struct ItemRecord *Item_asItemRecord(struct Database *db, struct Item *item)
{
    check(db != NULL, "Database is NULL");
    check(item != NULL, "Item is NULL");

    struct DescriptionRecord *descriptionRecord = Item_getDescriptionRecord(db, item);
    check(descriptionRecord != NULL, "Failed to get description record.");

    struct ItemRecord *record = Database_getItemByName(db, item->name);
    if (record == NULL) {
        record = malloc(sizeof(struct ItemRecord));
        check_mem(record);

        record->id = 0;
        strncpy(record->name, item->name, MAX_NAME);
        record->name[MAX_NAME - 1] = '\0';
        record->description_id = descriptionRecord->id;
    }

    return record;

error:
    return NULL;
}

int Item_save(struct Database *db, struct Item *item)
{
    check(db != NULL, "Database is NULL");

    if (item == NULL) {
        log_err("Item is NULL");
        return -1;
    }
    if (strcmp(item->name, "") == 0) {
        log_err("Item name is empty");
        return -1;
    }

    if (item->id == 0) {
        // Item does not exist in DB, create it.
        int nextItemID = Database_getNextIndex(db, ITEMS);
        item->id = nextItemID;
        struct ItemRecord *record = Item_asItemRecord(db, item);
        check(record != NULL, "Failed to create item record.");
        record->id = nextItemID;

        // Create associated records
        struct DescriptionRecord *descriptionRecord = Item_getDescriptionRecord(db, item);
        check(descriptionRecord != NULL, "Failed to get description record.");
        record->description_id = descriptionRecord->id;

        enum MorkResult res = Database_createItem(db, record);
        free(record); // Cleanup

        if (res != MORK_OK) {
            log_err("Failed to create item record.");
            return -1;
        } else {
            return item->id;
        }
    } else {
        // Item exists in DB, update it.
        struct ItemRecord *record = Item_asItemRecord(db, item);
        check(record != NULL, "Failed to create item record.");

        // Ensure we update associated records too
        struct DescriptionRecord *descriptionRecord = Item_getDescriptionRecord(db, item);
        check(descriptionRecord != NULL, "Failed to get description record.");

        if (strcmp(descriptionRecord->description, item->description) != 0) {
            // Description has changed, update it
            memset(descriptionRecord->description, 0, MAX_DESCRIPTION);
            strncpy(descriptionRecord->description, item->description, MAX_DESCRIPTION);
            enum MorkResult descriptionUpdateResult = Database_updateDescription(db, descriptionRecord);
            if (descriptionUpdateResult != MORK_OK) {
                log_err("Failed to update description record.");
                return -1;
            }
        }

        record->description_id = descriptionRecord->id;

        enum MorkResult res = Database_updateItem(db, record);
        if (res != MORK_OK) {
            log_err("Failed to update item record.");
            return -1;
        } 

        return item->id;
    }

error:
    return -1;
}

struct Item* Item_load(struct Database* db, int id)
{
    check(id > 0, "Invalid ID given: %d", id);
    check(db != NULL, "Database is NULL");

    struct ItemRecord *record = Database_getItem(db, id);
    check(record != NULL, "Failed to load item.");

    struct DescriptionRecord *description = Database_getDescription(db, record->description_id);
    check(description != NULL, "Failed to load description.");

    struct Item *item = Item_create(record->name, description->description);
    item->id = record->id;

    return item;

error:
    return NULL;
}

struct Item *Item_loadByName(struct Database *db, char *name)
{
    check(name != NULL, "Expected a non-null name.");
    check(strcmp(name, "") != 0, "Expected a non-empty name.");
    check(db != NULL, "Expected a non-null database.");

    struct ItemRecord *record = Database_getItemByName(db, name);
    check(record != NULL, "Failed to load item.");

    return Item_load(db, record->id);

error:
    return NULL;
}
