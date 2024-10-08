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

#include "../../utils/error.h"

#define MAX_INVENTORY_ITEMS 256
#define MAX_ROWS_INVENTORIES 65535

struct InventoryRecord {
    unsigned short id;
    unsigned char set;
    unsigned short owner_id;
    unsigned short item_ids[MAX_INVENTORY_ITEMS];
};

struct InventoryRecord* InventoryRecord_create(unsigned short id, unsigned short owner_id);
enum MorkResult InventoryRecord_destroy(struct InventoryRecord* record);
enum MorkResult InventoryRecord_addItem(struct InventoryRecord* record, unsigned short item_id);
enum MorkResult InventoryRecord_removeItem(struct InventoryRecord* record, unsigned short item_id);
unsigned short InventoryRecord_getItemCount(struct InventoryRecord* record);
unsigned short InventoryRecord_getID(struct InventoryRecord* record);
unsigned short InventoryRecord_getOwnerID(struct InventoryRecord* record);

struct InventoryTable {
    struct InventoryRecord rows[MAX_ROWS_INVENTORIES];
};

struct InventoryTable* InventoryTable_create();
enum MorkResult InventoryTable_init(struct InventoryTable* table);
enum MorkResult InventoryTable_destroy(struct InventoryTable* table);
enum MorkResult InventoryTable_add(struct InventoryTable* table, unsigned short owner_id, int junction_id);
enum MorkResult InventoryTable_update(struct InventoryTable *table, struct InventoryRecord *record);
enum MorkResult InventoryTable_remove(struct InventoryTable* table, unsigned short id);
struct InventoryRecord* InventoryTable_get(struct InventoryTable* table, unsigned short id);
struct InventoryRecord* InventoryTable_getByOwner(struct InventoryTable* table, unsigned short owner_id);

enum MorkResult InventoryTable_print(struct InventoryTable* table);