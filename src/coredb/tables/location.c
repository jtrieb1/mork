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

#include <lcthw/dbg.h>
#include <stdlib.h>
#include <string.h>

struct LocationRecord *LocationRecord_create(
    char *name,
    unsigned short descriptionID
)
{
    check(name != NULL && strcmp(name, "") != 0, "Expected a valid name");
    check(descriptionID != 0, "Expected a valid ID");

    struct LocationRecord *record = (struct LocationRecord *)calloc(1, sizeof(struct LocationRecord));
    record->name = name;
    record->descriptionID = descriptionID;

    memset(record->exitIDs, MAX_EXITS, sizeof(unsigned short));
    memset(record->itemIDs, MAX_ITEMS, sizeof(unsigned short));
    memset(record->characterIDs, MAX_CHARACTERS, sizeof(unsigned short));
    return record;

error:
    return NULL;
}

enum MorkResult LocationRecord_destroy(struct LocationRecord *record)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    if (record->name) free(record->name);
    if (record) free(record);
    
    return MORK_OK;
}

struct LocationRecord *LocationRecord_copy(struct LocationRecord *record)
{
    check(record != NULL, "Expected valid LocationRecord");

    struct LocationRecord *copy = (struct LocationRecord *)calloc(1, sizeof(struct LocationRecord));
    copy->name = strdup(record->name);
    copy->descriptionID = record->descriptionID;

    memcpy(copy->exitIDs, record->exitIDs, MAX_EXITS * sizeof(unsigned short));
    memcpy(copy->itemIDs, record->itemIDs, MAX_ITEMS * sizeof(unsigned short));
    memcpy(copy->characterIDs, record->characterIDs, MAX_CHARACTERS * sizeof(unsigned short));
    return copy;

error:
    return NULL;
}

enum MorkResult LocationRecord_setName(struct LocationRecord *record, char *name)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (name == NULL || strcmp(name, "") == 0) { return MORK_ERROR_DB_INVALID_DATA; }

    if (record->name) free(record->name);
    record->name = name != NULL ? name : "";

    return MORK_OK;
}

enum MorkResult LocationRecord_setDescriptionID(struct LocationRecord *record, unsigned short descriptionID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (descriptionID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    record->descriptionID = descriptionID;

    return MORK_OK;
}

enum MorkResult LocationRecord_addExitID(struct LocationRecord *record, unsigned short exitID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (exitID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_EXITS; i++)
    {
        if (record->exitIDs[i] == 0)
        {
            record->exitIDs[i] = exitID;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_FIELD_FULL;
}

enum MorkResult LocationRecord_addItemID(struct LocationRecord *record, unsigned short itemID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (itemID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (record->itemIDs[i] == 0)
        {
            record->itemIDs[i] = itemID;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_FIELD_FULL;
}

enum MorkResult LocationRecord_addCharacterID(struct LocationRecord *record, unsigned short characterID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (characterID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        if (record->characterIDs[i] == 0)
        {
            record->characterIDs[i] = characterID;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_FIELD_FULL;
}

enum MorkResult LocationRecord_removeExitID(struct LocationRecord *record, unsigned short exitID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (exitID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_EXITS; i++)
    {
        if (record->exitIDs[i] == exitID)
        {
            record->exitIDs[i] = 0;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}

enum MorkResult LocationRecord_removeItemID(struct LocationRecord *record, unsigned short itemID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (itemID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (record->itemIDs[i] == itemID)
        {
            record->itemIDs[i] = 0;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}

enum MorkResult LocationRecord_removeCharacterID(struct LocationRecord *record, unsigned short characterID)
{
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }
    if (characterID == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        if (record->characterIDs[i] == characterID)
        {
            record->characterIDs[i] = 0;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
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

enum MorkResult LocationTable_destroy(struct LocationTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    free(table);
    return MORK_OK;
}

enum MorkResult LocationTable_add(struct LocationTable *table, struct LocationRecord *record)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    int next_idx = findNextRowToFill(table->locations, MAX_LOCATIONS);
    memcpy(&table->locations[next_idx], record, sizeof(struct LocationRecord));

    return MORK_OK;
}

enum MorkResult LocationTable_update(struct LocationTable *table, struct LocationRecord *record)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (record == NULL) { return MORK_ERROR_DB_RECORD_NULL; }

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == record->id)
        {
            memcpy(&table->locations[i], record, sizeof(struct LocationRecord));
            return MORK_OK;
        }
    }

    return LocationTable_add(table, record);
}

struct LocationRecord *LocationTable_get(struct LocationTable *table, unsigned short id)
{
    check(table != NULL, "Expected a valid table");
    check(id != 0, "Invalid ID given: 0");

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == id)
        {
            return &table->locations[i];
        }
    }

error:
    return NULL;
}

enum MorkResult LocationTable_remove(struct LocationTable *table, unsigned short id)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }
    if (id == 0) { return MORK_ERROR_DB_INVALID_ID; }

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (table->locations[i].id == id)
        {
            LocationRecord_destroy(&table->locations[i]);
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}

struct LocationRecord *LocationTable_getByName(struct LocationTable *table, char *name)
{
    check(table != NULL, "Expected a valid table, got NULL");
    check(name != NULL && strcmp(name, "") != 0, "Expected a name");

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        if (strcmp(table->locations[i].name, name) == 0)
        {
            return &table->locations[i];
        }
    }

error:
    return NULL;
}

enum MorkResult LocationTable_print(struct LocationTable *table)
{
    if (table == NULL) { return MORK_ERROR_DB_TABLE_NULL; }

    for (int i = 0; i < MAX_LOCATIONS; i++)
    {
        struct LocationRecord *record = &table->locations[i];
        if (record->id != 0)
        {
            log_info("ID: %d, Name: %s, Description ID: %d", record->id, record->name, record->descriptionID);
        }
    }

    return MORK_OK;
}
