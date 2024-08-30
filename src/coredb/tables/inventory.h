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

#define MAX_INVENTORY_ITEMS 256
#define MAX_ROWS_INVENTORIES 65535

struct InventoryRecord {
    unsigned short id;
    unsigned short owner_id;
    unsigned short item_ids[MAX_INVENTORY_ITEMS];
};

struct InventoryRecord* InventoryRecord_create(unsigned short id, unsigned short owner_id);
void InventoryRecord_destroy(struct InventoryRecord* record);
void InventoryRecord_add_item(struct InventoryRecord* record, unsigned short item_id);
void InventoryRecord_remove_item(struct InventoryRecord* record, unsigned short item_id);
unsigned short InventoryRecord_get_item_count(struct InventoryRecord* record);
unsigned short InventoryRecord_get_item(struct InventoryRecord* record, unsigned short index);
unsigned short InventoryRecord_get_id(struct InventoryRecord* record);
unsigned short InventoryRecord_get_owner_id(struct InventoryRecord* record);

struct InventoryTable {
    unsigned short nextEmptyRow;
    unsigned short maxOccupiedRow;
    struct InventoryRecord rows[MAX_ROWS_INVENTORIES];
};

struct InventoryTable* InventoryTable_create();
void InventoryTable_init(struct InventoryTable* table);
void InventoryTable_destroy(struct InventoryTable* table);
unsigned short InventoryTable_add(struct InventoryTable* table, unsigned short owner_id);
void InventoryTable_remove(struct InventoryTable* table, unsigned short id);
struct InventoryRecord* InventoryTable_get(struct InventoryTable* table, unsigned short id);
struct InventoryRecord* InventoryTable_get_by_owner(struct InventoryTable* table, unsigned short owner_id);
