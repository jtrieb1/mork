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

#pragma once

#include "item.h"
#include "../utils/error.h"
#include "../coredb/db.h"

struct Inventory {
    struct Item *items[MAX_INVENTORY_ITEMS];
};

struct Inventory *Inventory_create();
enum MorkResult Inventory_destroy(struct Inventory *inventory);

struct Inventory *Inventory_clone(struct Inventory *source);

enum MorkResult Inventory_addItem(struct Inventory *inventory, struct Item *item);
enum MorkResult Inventory_removeItem(struct Inventory *inventory, struct Item *item);
int Inventory_hasItem(struct Inventory *inventory, struct Item *item);
int Inventory_isFull(struct Inventory *inventory);
int Inventory_isEmpty(struct Inventory *inventory);
int Inventory_getItemCount(struct Inventory *inventory);
struct Item *Inventory_getItem(struct Inventory *inventory, int index);
struct Item *Inventory_getItemByName(struct Inventory *inventory, const char *name);
enum MorkResult Inventory_print(struct Inventory *inventory);

int Inventory_save(struct Database *db, int owner_id, struct Inventory *inventory);
struct Inventory *Inventory_load(struct Database *db, int owner_id);
