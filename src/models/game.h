#pragma once

#include "../coredb/db.h"
#include "../utils/error.h"
#include "character.h"
#include "location.h"

struct BaseGame {
    unsigned short id;
    struct Character *player;
    struct Location *current_location;
};

struct BaseGame *BaseGame_create(struct Character *player);
enum MorkResult BaseGame_destroy(struct BaseGame *game);

enum MorkResult BaseGame_setPlayer(struct BaseGame *game, struct Character *player);
enum MorkResult BaseGame_setLocation(struct BaseGame *game, struct Location *location);

struct Character *BaseGame_getPlayer(struct BaseGame *game);
struct Location *BaseGame_getLocation(struct BaseGame *game);

enum MorkResult BaseGame_save(struct Database *db, struct BaseGame *game);
struct BaseGame *BaseGame_load(struct Database *db, int id);