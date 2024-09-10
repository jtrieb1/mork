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

#include "../coredb/db.h"
#include "../utils/error.h"

struct Item {
    unsigned short id;
    char name[MAX_NAME], description[MAX_DESCRIPTION];
};

struct Item *Item_create(const char* name, const char* description);
enum MorkResult Item_destroy(struct Item* item);

struct Item *Item_clone(struct Item *source);

// Returns ID of item on success, -1 on failure
int Item_save(struct Database* db, struct Item* item);
struct Item *Item_load(struct Database* db, int id);
struct Item *Item_loadByName(struct Database* db, char* name);
