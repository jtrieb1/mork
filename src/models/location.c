#include "location.h"

struct Location *Location_create(char *name, char *description)
{
    struct Location *location = (struct Location *)calloc(1, sizeof(struct Location));
    location->name = name;
    location->description = description;

    memset(location->exitIDs, MAX_EXITS, sizeof(int));
    memset(location->items, MAX_ITEMS, sizeof(struct Item));
    memset(location->characters, MAX_CHARACTERS, sizeof(struct Character));
    return location;
}

void Location_destroy(struct Location *location)
{
    if (location) free(location);
}
