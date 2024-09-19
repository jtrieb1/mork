#include "games.h"
#include "row.h"

#include <lcthw/dbg.h>

struct GameRecord *GameRecord_create(unsigned short id, unsigned short owner_id, unsigned short location_id)
{
    check(id > 0, "Expected a valid ID");
    check(owner_id > 0, "Expected a valid Owner ID");
    check(location_id > 0, "Expected a valid Location ID");

    struct GameRecord *record = calloc(1, sizeof(struct GameRecord));
    check_mem(record);

    record->id = id;
    record->set = 1;
    record->owner_id = owner_id;
    record->location_id = location_id;

    return record;

error:
    return NULL;
}

enum MorkResult GameRecord_destroy(struct GameRecord *record)
{
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    free(record);
    return MORK_OK;
}

enum MorkResult GameRecord_setOwnerID(struct GameRecord *record, unsigned short owner_id)
{
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    record->owner_id = owner_id;
    return MORK_OK;
}

enum MorkResult GameRecord_setLocationID(struct GameRecord *record, unsigned short location_id)
{
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    record->location_id = location_id;
    return MORK_OK;
}

enum MorkResult GameRecord_print(struct GameRecord *record)
{
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }
    printf("Game Record: ID: %d, Owner ID: %d, Location ID: %d\n", record->id, record->owner_id, record->location_id);
    return MORK_OK;
}

enum MorkResult GameTable_init(struct GameTable *table)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }

    for (int i = 0; i < MAX_ROWS_GAMES; i++) {
        table->rows[i].id = 0;
        table->rows[i].set = 0;
    }
    return MORK_OK;
}

struct GameTable *GameTable_create()
{
    struct GameTable *table = calloc(1, sizeof(struct GameTable));
    check_mem(table);

    GameTable_init(table);
    return table;

error:
    return NULL;
}

enum MorkResult GameTable_destroy(struct GameTable *table)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }
    free(table);
    return MORK_OK;
}

struct GameRecord *GameTable_get(struct GameTable *table, unsigned short id)
{
    check(id > 0, "ID is not set");
    check(table != NULL, "Table is NULL");

    for (unsigned short i = 0; i < MAX_ROWS_GAMES; i++) {
        struct GameRecord *rec = &table->rows[i];
        if (rec->id == id) {
            return rec;
        }
    }

error:
    return NULL;
}

enum MorkResult GameTable_insert(struct GameTable *table, struct GameRecord *record)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }

    record->set = 1;
    unsigned short idx = findNextRowToFill(table->rows, MAX_ROWS_GAMES);

    memcpy(&table->rows[idx], record, sizeof(struct GameRecord));
    table->rows[idx].set = 1;

    return MORK_OK;
}

enum MorkResult GameTable_update(struct GameTable *table, struct GameRecord *record)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }
    if (record == NULL) {
        return MORK_ERROR_DB_RECORD_NULL;
    }

    for (int i = 0; i < MAX_ROWS_GAMES; i++) {
        if (table->rows[i].id == record->id) {
            memcpy(&table->rows[i], record, sizeof(struct GameRecord));
            table->rows[i].set = 1;
            return MORK_OK;
        }
    }

    return GameTable_insert(table, record);
}

enum MorkResult GameTable_delete(struct GameTable *table, unsigned short id)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }

    for (int i = 0; i < MAX_ROWS_GAMES; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].id = 0;
            table->rows[i].set = 0;
            return MORK_OK;
        }
    }

    return MORK_ERROR_DB_NOT_FOUND;
}

enum MorkResult GameTable_print(struct GameTable *table)
{
    if (table == NULL) {
        return MORK_ERROR_DB_TABLE_NULL;
    }

    for (int i = 0; i < MAX_ROWS_GAMES; i++) {
        struct GameRecord *row = &table->rows[i];
        if (row->set == 1) {
            if (GameRecord_print(row) != MORK_OK) {
                return MORK_ERROR_DB;
            }
        }
        printf("\n");
    }
    return MORK_OK;
}