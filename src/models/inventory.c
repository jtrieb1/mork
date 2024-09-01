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

#include "inventory.h"
#include "item.h"

#include <stdlib.h>
#include <lcthw/dbg.h>

struct Inventory *Inventory_create()
{
    struct Inventory *inventory = calloc(1, sizeof(struct Inventory));
    check_mem(inventory);

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        inventory->items[i] = NULL;
    }

    return inventory;

error:

    return NULL;
}

struct Inventory *Inventory_clone(struct Inventory *source)
{
    struct Inventory *inventory = Inventory_create();
    check_mem(inventory);

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (source->items[i] != NULL) {
            inventory->items[i] = Item_clone(source->items[i]);
        }
    }

    return inventory;

error:
    return NULL;
}

void Inventory_destroy(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            Item_destroy(inventory->items[i]);
        }
    }

    free(inventory);
}

int Inventory_addItem(struct Inventory *inventory, struct Item *item)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] == NULL) {
            inventory->items[i] = item;
            return 0;
        }
    }

    return -1;
}

int Inventory_removeItem(struct Inventory *inventory, struct Item *item)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (strncmp(inventory->items[i]->name, item->name, MAX_NAME - 1) == 0) {
            Item_destroy(inventory->items[i]);
            inventory->items[i] = NULL;
            return 0;
        }
    }

    return -1;
}

int Inventory_hasItem(struct Inventory *inventory, struct Item *item)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] == item) {
            return 1;
        }
    }

    return 0;
}

int Inventory_isFull(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] == NULL) {
            return 0;
        }
    }

    return 1;
}

int Inventory_isEmpty(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            return 0;
        }
    }

    return 1;
}

int Inventory_getItemCount(struct Inventory *inventory)
{
    int count = 0;
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            count++;
        }
    }

    return count;
}

struct Item *Inventory_getItem(struct Inventory *inventory, int index)
{
    if (index < 0 || index >= MAX_INVENTORY_ITEMS) {
        return NULL;
    }

    return inventory->items[index];
}

struct Item *Inventory_getItemByName(struct Inventory *inventory, const char *name)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL && strncmp(inventory->items[i]->name, name, MAX_NAME) == 0) {
            return inventory->items[i];
        }
    }

    return NULL;
}

void Inventory_print(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            printf("%s\n", inventory->items[i]->name);
        }
    }
}

int Inventory_save(struct Database *db, int owner_id, struct Inventory *inventory)
{
    struct InventoryRecord *record = NULL;
    struct CharacterStatsRecord *owner = Database_getCharacterStats(db, owner_id);

    check(owner != NULL, "Failed to get owner record");
    record = Database_getInventoryByOwner(db, owner->name);
    if (record == NULL) {
        return Database_createInventory(db, owner->name);
    } else {
        for (int i = 0; i < MAX_INVENTORY_ITEMS; ++i) {
            if (inventory->items[i] != NULL) {
                record->item_ids[i] = Item_save(db, inventory->items[i]);
            } else {
                record->item_ids[i] = 0;
            }
        }
        return Database_updateInventory(db, record, record->id);
    }

error:
    return -1;
}

struct Inventory *Inventory_load(struct Database *db, int owner_id)
{
    struct CharacterStatsRecord *owner = Database_getCharacterStats(db, owner_id);
    struct InventoryRecord *record = Database_getInventoryByOwner(db, owner->name);
    check(record != NULL, "Inventory record does not exist in database");

    struct Inventory *inventory = Inventory_create();

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            inventory->items[i] = Item_load(db, record->item_ids[i]);
        }
    }

    return inventory;

error:
    return NULL;
}
