#pragma once

#include "../utils/error.h"

#include "character.h"
#include "item.h"

#include <stdlib.h>
#include <stdio.h>

enum ExitDirection {
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3,
    UP = 4,
    DOWN = 5
};

enum ExitDirection oppositeDirection(enum ExitDirection direction);

struct Location {
    unsigned short id;
    char *name;
    char *description;
    int exitIDs[MAX_EXITS];
    struct Item *items[MAX_ITEMS];
    struct Character *characters[MAX_CHARACTERS];
};

struct Location *Location_create(char *name, char *description);
enum MorkResult Location_destroy(struct Location *location);

enum MorkResult Location_setName(struct Location *location, char *name);
enum MorkResult Location_setDescription(struct Location *location, char *description);

enum MorkResult Location_addExit(struct Location *from, enum ExitDirection direction, struct Location *to);
enum MorkResult Location_removeExit(struct Location *from, enum ExitDirection direction);

enum MorkResult Location_addItem(struct Location *location, struct Item *item);
enum MorkResult Location_removeItem(struct Location *location, struct Item *item);
int Location_getItemCount(struct Location *location);

enum MorkResult Location_addCharacter(struct Location *location, struct Character *character);
enum MorkResult Location_removeCharacter(struct Location *location, struct Character *character);

enum MorkResult Location_save(struct Database *db, struct Location *location);
struct Location *Location_load(struct Database *db, int id);
struct Location *Location_loadByName(struct Database *db, char *name);