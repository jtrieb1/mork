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

#include "location.h"
#include "row.h"

#include <stdlib.h>
#include <string.h>

struct LocationRecord *LocationRecord_create(
    char *name,
    unsigned short descriptionID
)
{
    struct LocationRecord *record = (struct LocationRecord *)calloc(1, sizeof(struct LocationRecord));
    record->name = name;
    record->descriptionID = descriptionID;

    memset(record->exitIDs, MAX_EXITS, sizeof(unsigned short));
    memset(record->itemIDs, MAX_ITEMS, sizeof(unsigned short));
    memset(record->characterIDs, MAX_CHARACTERS, sizeof(unsigned short));
    return record;
}

void LocationRecord_destroy(struct LocationRecord *record)
{
    if (record) free(record->name);
    if (record) free(record);
}

struct LocationRecord *LocationRecord_copy(struct LocationRecord *record)
{
    struct LocationRecord *copy = (struct LocationRecord *)calloc(1, sizeof(struct LocationRecord));
    copy->name = strdup(record->name);
    copy->descriptionID = record->descriptionID;

    memcpy(copy->exitIDs, record->exitIDs, MAX_EXITS * sizeof(unsigned short));
    memcpy(copy->itemIDs, record->itemIDs, MAX_ITEMS * sizeof(unsigned short));
    memcpy(copy->characterIDs, record->characterIDs, MAX_CHARACTERS * sizeof(unsigned short));
    return copy;
}

void LocationRecord_setName(struct LocationRecord *record, char *name)
{
    free(record->name);
    record->name = name;
}

void LocationRecord_setDescriptionID(struct LocationRecord *record, unsigned short descriptionID)
{
    record->descriptionID = descriptionID;
}

void LocationRecord_addExitID(struct LocationRecord *record, unsigned short exitID)
{
    for (int i = 0; i < MAX_EXITS; i++)
    {
        if (record->exitIDs[i] == 0)
        {
            record->exitIDs[i] = exitID;
            return;
        }
    }
}

void LocationRecord_addItemID(struct LocationRecord *record, unsigned short itemID)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (record->itemIDs[i] == 0)
        {
            record->itemIDs[i] = itemID;
            return;
        }
    }
}

void LocationRecord_addCharacterID(struct LocationRecord *record, unsigned short characterID)
{
    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        if (record->characterIDs[i] == 0)
        {
            record->characterIDs[i] = characterID;
            return;
        }
    }
}

void LocationRecord_removeExitID(struct LocationRecord *record, unsigned short exitID)
{
    for (int i = 0; i < MAX_EXITS; i++)
    {
        if (record->exitIDs[i] == exitID)
        {
            record->exitIDs[i] = 0;
            return;
        }
    }
}

void LocationRecord_removeItemID(struct LocationRecord *record, unsigned short itemID)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (record->itemIDs[i] == itemID)
        {
            record->itemIDs[i] = 0;
            return;
        }
    }
}

void LocationRecord_removeCharacterID(struct LocationRecord *record, unsigned short characterID)
{
    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        if (record->characterIDs[i] == characterID)
        {
            record->characterIDs[i] = 0;
            return;
        }
    }
}

struct LocationTable *LocationTable_create()
{
    struct LocationTable *table = (struct LocationTable *)calloc(1, sizeof(struct LocationTable));
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        table->locations[i] = (struct LocationRecord){0};
    }
    return table;
}

void LocationTable_destroy(struct LocationTable *table)
{
    free(table);
}

unsigned short LocationTable_add(struct LocationTable *table, struct LocationRecord *record)
{
    int next_idx = findNextRowToFill(table->locations, MAX_LOCATIONS);
    memcpy(&table->locations[next_idx], record, sizeof(struct LocationRecord));
    return table->locations[next_idx].id;
}

unsigned short LocationTable_update(struct LocationTable *table, struct LocationRecord *record, unsigned short id)
{
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == id)
        {
            memcpy(&table->locations[i], record, sizeof(struct LocationRecord));
            return id;
        }
    }
    return 0;
}

struct LocationRecord *LocationTable_get(struct LocationTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == id)
        {
            return &table->locations[i];
        }
    }
    return NULL;
}

void LocationTable_remove(struct LocationTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == id)
        {
            LocationRecord_destroy(&table->locations[i]);
            return;
        }
    }
}

struct LocationRecord *LocationTable_getByName(struct LocationTable *table, char *name)
{
    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (strcmp(table->locations[i].name, name) == 0)
        {
            return &table->locations[i];
        }
    }
    return NULL;
}
