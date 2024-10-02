#pragma once

#include "../coredb/db.h"
#include "../utils/error.h"
#include "action.h"
#include "character.h"
#include "location.h"
#include "../ui/terminal.h"

#define MAX_HISTORY 100

struct BaseGame {
    unsigned short id;
    struct ScreenState *screen;
    struct Action *history[MAX_HISTORY];
    struct Character *player;
    struct Location *current_location;
};

struct BaseGame *BaseGame_create(struct Character *player);
enum MorkResult BaseGame_destroy(struct BaseGame *game);

enum MorkResult BaseGame_setHeader(struct BaseGame *game, struct TerminalSegment *header);
enum MorkResult BaseGame_setBody(struct BaseGame *game, struct TerminalSegment *body);
enum MorkResult BaseGame_setStatusBar(struct BaseGame *game, struct TerminalSegment *statusBar);

enum MorkResult BaseGame_setPlayer(struct BaseGame *game, struct Character *player);
enum MorkResult BaseGame_setLocation(struct BaseGame *game, struct Location *location);

struct Character *BaseGame_getPlayer(struct BaseGame *game);
struct Location *BaseGame_getLocation(struct BaseGame *game);

enum MorkResult BaseGame_save(struct Database *db, struct BaseGame *game);
struct BaseGame *BaseGame_load(struct Database *db, int id);

// This method returns the string that should be printed to the user
struct TerminalSegment *BaseGame_execute(struct Database *db, struct BaseGame *game, struct Action *action);
enum MorkResult BaseGame_run(struct Database *db, struct BaseGame *game);

// Lower level methods

enum MorkResult BaseGame_executeAction(struct Database *db, struct BaseGame *game, struct Action *action);
char *BaseGame_getScreenDisplay(struct BaseGame *game);