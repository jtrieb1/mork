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
#include "../../utils/error.h"

#define MAX_EXITS 6
#define MAX_ITEMS 10
#define MAX_CHARACTERS 10
#define MAX_LOCATIONS 100

struct LocationRecord {
    unsigned char id;
    unsigned char set;
    char *name;
    unsigned short descriptionID;
    unsigned short exitIDs[MAX_EXITS];
    unsigned short itemIDs[MAX_ITEMS];
    unsigned short characterIDs[MAX_CHARACTERS];
};

struct LocationRecord *LocationRecord_create(
    char *name,
    unsigned short descriptionID
);

enum MorkResult LocationRecord_destroy(struct LocationRecord *record);

struct LocationRecord *LocationRecord_copy(struct LocationRecord *record);

enum MorkResult LocationRecord_setName(struct LocationRecord *record, char *name);
enum MorkResult LocationRecord_setDescriptionID(struct LocationRecord *record, unsigned short descriptionID);
enum MorkResult LocationRecord_addExitID(struct LocationRecord *record, unsigned short exitID);
enum MorkResult LocationRecord_addItemID(struct LocationRecord *record, unsigned short itemID);
enum MorkResult LocationRecord_addCharacterID(struct LocationRecord *record, unsigned short characterID);
enum MorkResult LocationRecord_removeExitID(struct LocationRecord *record, unsigned short exitID);
enum MorkResult LocationRecord_removeItemID(struct LocationRecord *record, unsigned short itemID);
enum MorkResult LocationRecord_removeCharacterID(struct LocationRecord *record, unsigned short characterID);

struct LocationTable {
    struct LocationRecord locations[MAX_LOCATIONS];
};

struct LocationTable *LocationTable_create();
enum MorkResult LocationTable_destroy(struct LocationTable *table);

enum MorkResult LocationTable_add(struct LocationTable *table, struct LocationRecord *record);
enum MorkResult LocationTable_update(struct LocationTable *table, struct LocationRecord *record);
struct LocationRecord *LocationTable_get(struct LocationTable *table, unsigned short id);
enum MorkResult LocationTable_remove(struct LocationTable *table, unsigned short id);
struct LocationRecord *LocationTable_getByName(struct LocationTable *table, char *name);

enum MorkResult LocationTable_print(struct LocationTable *table);