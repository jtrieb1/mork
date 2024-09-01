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

void Item_destroy(struct Item *item)
{
    if (item != NULL) free(item);
    item = NULL;
}

int Item_save(struct Database *db, struct Item *item)
{
    // Try to find description record
    int description_id = -1;
    struct DescriptionRecord *descriptionRecord = Database_getDescriptionByPrefix(db, item->description);
    if (descriptionRecord == NULL) {
        // Description does not exist in DB, create it.
        descriptionRecord = DescriptionRecord_create(Database_getNextIndex(db, DESCRIPTION), item->description, 0);
        check(descriptionRecord != NULL, "Failed to create description record.");
        description_id = Database_createDescription(db, descriptionRecord);
        free(descriptionRecord);
    } else {
        description_id = descriptionRecord->id;
    }

    struct ItemRecord *itemRecord = Database_getItemByName(db, item->name);
    if (itemRecord == NULL) {
        // Item does not exist in DB, create it.
        itemRecord = ItemRecord_create(Database_getNextIndex(db, ITEMS), item->name, description_id);
        check(itemRecord != NULL, "Failed to create item record.");
        int id = Database_createItem(db, itemRecord);
        free(itemRecord);
        return id;
    } else {
        // Item exists in DB, update it.
        if (itemRecord->description_id != description_id) {
            itemRecord->description_id = description_id;
            return Database_updateItem(db, itemRecord, itemRecord->id);
        } else {
            return itemRecord->id;
        }
    }

error:
    if (itemRecord) {
        Database_deleteItem(db, itemRecord->id);
    }
    if (descriptionRecord) {
        Database_deleteDescription(db, descriptionRecord->id);
    }
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
