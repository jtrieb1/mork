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
    struct ItemRecord *record = Database_getOrCreateItem(db, item->name);
    struct DescriptionRecord *description = Database_getOrCreateDescription(db, item->description);

    record->description_id = description->id;

    return Database_updateItem(db, record, record->id);
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
