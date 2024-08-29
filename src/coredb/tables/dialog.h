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

#define MAX_TEXT 505 // To ensure we add to 512 bytes
#define MAX_ROWS_DIALOG 65535

struct DialogRecord {
    unsigned short id;                               // 2 bytes
    unsigned char set;                               // 1 byte
    char text[MAX_TEXT];                             // 505 bytes
    unsigned short next_id; // -1 if end of dialog   // 4 bytes
                                                     // 512 bytes
};

struct DialogRecord *DialogRecord_create(unsigned short id, char *dialog, int next_id);
void DialogRecord_destroy(struct DialogRecord *record);

struct DialogTable {
    unsigned short nextEmptyRow;
    unsigned short maxOccupiedRow;
    struct DialogRecord rows[MAX_ROWS_DIALOG]; // Total size on disk is 512 * 65535 = 33,553,920 bytes. A bit excessive for Zork, but this is Mork
};

struct DialogTable *DialogTable_create();
void DialogTable_init(struct DialogTable *table);
void DialogTable_destroy(struct DialogTable *table);
void DialogTable_set(struct DialogTable *table, struct DialogRecord *record);
struct DialogRecord *DialogTable_get(struct DialogTable *table, unsigned short id);
void DialogTable_delete(struct DialogTable *table, unsigned short id);
void DialogTable_print(struct DialogTable *table);
