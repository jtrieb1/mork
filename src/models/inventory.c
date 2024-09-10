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

    inventory->id = 0;

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

    inventory->id = source->id;

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (source->items[i] != NULL) {
            inventory->items[i] = Item_clone(source->items[i]);
        }
    }

    return inventory;

error:
    return NULL;
}

enum MorkResult Inventory_destroy(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            return Item_destroy(inventory->items[i]);
        }
    }

    free(inventory);
    return MORK_OK;
}

enum MorkResult Inventory_addItem(struct Inventory *inventory, struct Item *item)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] == NULL) {
            inventory->items[i] = item;
            return MORK_OK;
        }
    }

    return MORK_ERROR_MODEL_INVENTORY_FULL;
}

enum MorkResult Inventory_removeItem(struct Inventory *inventory, struct Item *item)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (strncmp(inventory->items[i]->name, item->name, MAX_NAME - 1) == 0) {
            Item_destroy(inventory->items[i]);
            inventory->items[i] = NULL;
            return MORK_OK;
        }
    }

    return MORK_ERROR_MODEL_INVENTORY_ITEM_NOT_FOUND;
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

enum MorkResult Inventory_print(struct Inventory *inventory)
{
    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            printf("%s\n", inventory->items[i]->name);
        }
    }
    return MORK_OK;
}

struct InventoryRecord *Inventory_asInventoryRecord(struct Database *db, struct Inventory *inventory, unsigned short owner_id)
{
    struct CharacterRecord *owner = Database_getCharacter(db, owner_id);
    struct InventoryRecord *record = Database_getInventoryByOwner(db, owner->name);
    if (record == NULL) {
        record = InventoryRecord_create(Database_getNextIndex(db, INVENTORY), owner_id);
    }

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (inventory->items[i] != NULL) {
            record->item_ids[i] = inventory->items[i]->id;
        }
    }

    return record;
}

int Inventory_save(struct Database *db, unsigned short owner, struct Inventory *inventory)
{
    struct CharacterRecord *owner_record = Database_getCharacter(db, owner);
    check(owner_record != NULL, "Failed to load character record.");
    struct InventoryRecord *record = Inventory_asInventoryRecord(db, inventory, owner);
    check(record != NULL, "Failed to create inventory record.");

    if (record->id == 0) {
        record->id = Database_getNextIndex(db, INVENTORY);
        enum MorkResult res = Database_createInventory(db, owner_record->name);
        check(res == MORK_OK, "Failed to create inventory record.");
    } else {
        enum MorkResult res = Database_updateInventory(db, record);
        check(res == MORK_OK, "Failed to update inventory record.");
    }

    return record->id;

error:
    return -1;
}

struct Inventory *Inventory_load(struct Database *db, int owner_id)
{
    struct CharacterRecord *owner = Database_getCharacter(db, owner_id);
    struct InventoryRecord *record = Database_getInventoryByOwner(db, owner->name);

    if (record == NULL) {
        return Inventory_create();
    }

    struct Inventory *inventory = Inventory_create();
    inventory->id = record->id;

    for (int i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (record->item_ids[i] != 0) {
            inventory->items[i] = Item_load(db, record->item_ids[i]);
        }
    }

    return inventory;
}
