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

#define MAX_DESCRIPTION 512
#define MAX_ROWS_DESC 65535

struct DescriptionRecord {
    unsigned short id;
    unsigned char set;
    char description[MAX_DESCRIPTION];
    int next_id;
};

struct DescriptionRecord *DescriptionRecord_create(unsigned short id, char *description, int next_id);
enum MorkResult DescriptionRecord_destroy(struct DescriptionRecord *entry);

struct DescriptionTable {
    struct DescriptionRecord rows[MAX_ROWS_DESC];
};

struct DescriptionTable *DescriptionTable_create();
enum MorkResult DescriptionTable_init(struct DescriptionTable *table);
enum MorkResult DescriptionTable_insert(struct DescriptionTable *table, struct DescriptionRecord *entry);
enum MorkResult DescriptionTable_update(struct DescriptionTable *table, struct DescriptionRecord *entry);
struct DescriptionRecord *DescriptionTable_get(struct DescriptionTable *table, unsigned short id);
struct DescriptionRecord *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id); // For easy continuation of text
struct DescriptionRecord *DescriptionTable_get_by_prefix(struct DescriptionTable *table, char *prefix);
enum MorkResult DescriptionTable_delete(struct DescriptionTable *table, unsigned short id);
enum MorkResult DescriptionTable_destroy(struct DescriptionTable *table);

enum MorkResult DescriptionTable_print(struct DescriptionTable *table);