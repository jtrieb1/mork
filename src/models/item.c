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
    return Item_create(source->name, source->description);
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
    }

    return descriptionRecord;

error:
    return NULL;
}

struct ItemRecord *Item_asItemRecord(struct Database *db, struct Item *item)
{
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

        enum MorkResult res = Database_createItem(db, record);
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
    struct ItemRecord *record = Database_getItem(db, id);
    check(record != NULL, "Failed to load item.");

    struct DescriptionRecord *description = Database_getDescription(db, record->description_id);
    check(description != NULL, "Failed to load description.");

    struct Item *item = Item_create(record->name, description->description);
    return item;

error:
    return NULL;
}

struct Item *Item_loadByName(struct Database *db, char *name)
{
    struct ItemRecord *record = Database_getItemByName(db, name);
    check(record != NULL, "Failed to load item.");

    struct DescriptionRecord *description = Database_getDescription(db, record->description_id);
    check(description != NULL, "Failed to load description.");

    struct Item *item = Item_create(record->name, description->description);
    return item;

error:
    return NULL;
}
