#pragma once

#include "../utils/error.h"

#include "character.h"
#include "item.h"

struct Location {
    char *name;
    char *description;
    int exitIDs[MAX_EXITS];
    struct Item items[MAX_ITEMS];
    struct Character characters[MAX_CHARACTERS];
};

struct Location *Location_create(char *name, char *description);
enum MorkResult Location_destroy(struct Location *location);

enum MorkResult Location_save(struct Database *db, struct Location *location);
struct Location *Location_load(struct Database *db, int id);