#pragma once

#define MAX_ROWS_GAMES 65535

#include "../../utils/error.h"

#include <stdlib.h>

struct GameRecord {
    unsigned short id;
    unsigned char set;
    unsigned short owner_id;
    unsigned short location_id;
};

struct GameRecord *GameRecord_create(unsigned short id, unsigned short owner_id, unsigned short location_id);
enum MorkResult GameRecord_destroy(struct GameRecord *record);

enum MorkResult GameRecord_setOwnerID(struct GameRecord *record, unsigned short owner_id);
enum MorkResult GameRecord_setLocationID(struct GameRecord *record, unsigned short location_id);
enum MorkResult GameRecord_print(struct GameRecord *record);

struct GameTable {
    struct GameRecord rows[MAX_ROWS_GAMES];
};

enum MorkResult GameTable_init(struct GameTable *table);
struct GameTable *GameTable_create();
enum MorkResult GameTable_destroy(struct GameTable *table);

struct GameRecord *GameTable_get(struct GameTable *table, unsigned short id);
enum MorkResult GameTable_insert(struct GameTable *table, struct GameRecord *record);
enum MorkResult GameTable_update(struct GameTable *table, struct GameRecord *record);
enum MorkResult GameTable_delete(struct GameTable *table, unsigned short id);

enum MorkResult GameTable_print(struct GameTable *table);