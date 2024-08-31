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

void Item_destroy(struct Item *item)
{
    free(item);
}

int Item_save(struct Database *db, struct Item *item)
{
    struct ItemRecord *record = Database_get_item_by_name(db, item->name);
    if (record == NULL) {
        // Create record
        record = malloc(sizeof(struct ItemRecord));
        check_mem(record);

        strncpy(record->name, item->name, MAX_NAME);
        record->name[MAX_NAME - 1] = '\0';

        record->set = 1;

        // This is new, so so is its description
        struct DescriptionRecord *description = malloc(sizeof(struct DescriptionRecord));
        check_mem(description);

        strncpy(description->description, item->description, MAX_DESCRIPTION);
        description->description[MAX_DESCRIPTION - 1] = '\0';

        description->set = 1;
        int description_id = Database_set_description(db, description);
        check(description_id != -1, "Failed to set description");

        record->description_id = description_id;

        return Database_set_item(db, record);
    }

    strncpy(record->name, item->name, MAX_NAME);
    record->name[MAX_NAME - 1] = '\0';

    struct DescriptionRecord *description = Database_get_description_by_prefix(db, item->description);
    if (description == NULL) {
        // Create description
        description = malloc(sizeof(struct DescriptionRecord));
        check_mem(description);

        strncpy(description->description, item->description, MAX_DESCRIPTION);
        description->description[MAX_DESCRIPTION - 1] = '\0';

        description->set = 1;

        int description_id = Database_set_description(db, description);
        check(description_id != -1, "Failed to set description");

        record->description_id = description_id;
    } else {
        record->description_id = description->id;
    }
    return Database_set_item(db, record);

error:
    return -1;
}

struct Item* Item_load(struct Database* db, int id)
{
    struct ItemRecord *record = Database_get_item(db, id);
    if (record == NULL) {
        return NULL;
    }

    struct DescriptionRecord *description = Database_get_description(db, record->description_id);
    if (description == NULL) {
        return NULL;
    }

    struct Item *item = Item_create(record->name, description->description);
    return item;
}
