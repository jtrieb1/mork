#pragma once

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
void Location_destroy(struct Location *location);
