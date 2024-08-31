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

#include <stdlib.h>
#include <lcthw/dbg.h>

struct Inventory *Inventory_create()
{
    struct Inventory *inventory = malloc(sizeof(struct Inventory));
    check_mem(inventory);

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        inventory->items[i] = NULL;
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
    if (Inventory_isFull(inventory)) {
        return -1;
    }

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
        if (inventory->items[i] == item) {
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

int Inventory_save(struct Database *db, char *owner_name, struct Inventory *inventory)
{
    struct InventoryRecord *record = Database_get_inventory_by_owner(db, owner_name);

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            Item_save(db, inventory->items[i]);
        }
    }
    int idx = Database_set_inventory(db, record);
    free(record);
    return idx;
}

struct Inventory *Inventory_load(struct Database *db, int id)
{
    struct InventoryRecord *record = Database_get_inventory(db, id);
    struct Inventory *inventory = Inventory_create();

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            inventory->items[i] = Item_load(db, record->item_ids[i]);
        }
    }

    free(record);
    return inventory;
}
