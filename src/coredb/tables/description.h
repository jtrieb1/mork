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

#define MAX_DESCRIPTION 512
#define MAX_ROWS_DESC 65535

struct DescriptionRecord {
    unsigned short id;
    unsigned char set;
    char description[MAX_DESCRIPTION];
    int next_id;
};

struct DescriptionRecord *DescriptionRecord_create(unsigned short id, char *description, int next_id);
void DescriptionRecord_destroy(struct DescriptionRecord *entry);

struct DescriptionTable {
    unsigned short nextEmptyRow;
    unsigned short maxOccupiedRow;
    struct DescriptionRecord rows[MAX_ROWS_DESC];
};

struct DescriptionTable *DescriptionTable_create();
void DescriptionTable_init(struct DescriptionTable *table);
unsigned short DescriptionTable_set(struct DescriptionTable *table, struct DescriptionRecord *entry);
struct DescriptionRecord *DescriptionTable_get(struct DescriptionTable *table, unsigned short id);
struct DescriptionRecord *DescriptionTable_get_next(struct DescriptionTable *table, unsigned short id); // For easy continuation of text
struct DescriptionRecord *DescriptionTable_get_by_prefix(struct DescriptionTable *table, char *prefix);
void DescriptionTable_delete(struct DescriptionTable *table, unsigned short id);
void DescriptionTable_destroy(struct DescriptionTable *table);
