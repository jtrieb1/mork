#include "game.h"
#include "../ui/terminal.h"

#include <lcthw/dbg.h>
#include <sys/types.h>

struct BaseGame *BaseGame_create(struct Character *player)
{

    struct BaseGame *game = calloc(1, sizeof(struct BaseGame));
    check_mem(game);

    game->player = player;
    game->current_location = NULL;
    game->screen = ScreenState_create();

    struct TerminalSegment *header = TS_new();
    check(header != NULL, "Failed to create header.");
    TS_concatText(TS_setGreen(TS_setBold(header)), "Mork");
    TS_destroy(game->screen->header);
    game->screen->header = header;

    struct TerminalSegment *statusBar = TS_new();
    check(statusBar != NULL, "Failed to create status bar.");
    TS_setCursorToScreenBottom(statusBar);
    TS_concatText(TS_setBold(TS_setWhite(statusBar)), "Health: ");
    TS_concatText(TS_setGreen(statusBar),              "100/100");
    TS_destroy(game->screen->statusBar);
    game->screen->statusBar = statusBar;

    for (int i = 0; i < MAX_HISTORY; i++) {
        game->history[i] = NULL;
    }

    return game;

error:
    return NULL;
}

enum MorkResult BaseGame_refreshScreen(struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    
    ScreenState_clear();
    ScreenState_print(game->screen);

    return MORK_OK;

error:
    return MORK_ERROR_MODEL_GAME_NULL;
}

enum MorkResult BaseGame_executeAction(struct Database *db, struct BaseGame *game, struct Action *action)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct TerminalSegment *result = BaseGame_execute(db, game, action);
    
    if (result != NULL) {
        // This adds to the text onscreen, not overwriting context lines
        ScreenState_textAppend(game->screen, result);

        // Update our status bar
        char *playerHealth = calloc(1, 10);
        check_mem(playerHealth);
        sprintf(playerHealth, "%hu/%hu", game->player->health, game->player->max_health);

        ScreenState_statusBarSet(game->screen, "Health: ");
        struct TerminalSegment *green = TS_setGreen(TS_new());
        ScreenState_statusBarAppendInline(game->screen, TS_concatText(green, playerHealth));
        free(playerHealth);

        // Append to history
        for (int i = MAX_HISTORY - 1; i > 0; i--) {
            game->history[i] = game->history[i - 1];
        }
        game->history[0] = action;
    }

    return MORK_OK;

error:
    return MORK_ERROR_MODEL_GAME_NULL;
}

enum MorkResult BaseGame_destroy(struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    Character_destroy(game->player);
    game->player = NULL;
    Location_destroy(game->current_location);
    game->current_location = NULL;
    ScreenState_destroy(game->screen);
    game->screen = NULL;

    for (int i = 0; i < MAX_HISTORY; i++) {
        if (game->history[i] != NULL) {
            Action_destroy(game->history[i]);
            game->history[i] = NULL;
        }
    }

    free(game);
    return MORK_OK;
}

enum MorkResult BaseGame_setHeader(struct BaseGame *game, struct TerminalSegment *header)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    ScreenState_headerSet(game->screen, header->rawTextRepresentation);
    return MORK_OK;
}

enum MorkResult BaseGame_setPlayer(struct BaseGame *game, struct Character *player)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    game->player = player;
    return MORK_OK;
}

enum MorkResult BaseGame_setLocation(struct BaseGame *game, struct Location *location)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    game->current_location = location;
    return MORK_OK;
}

struct Character *BaseGame_getPlayer(struct BaseGame *game)
{
    if (game == NULL) {
        return NULL;
    }
    return game->player;
}

struct Location *BaseGame_getLocation(struct BaseGame *game)
{
    if (game == NULL) {
        return NULL;
    }
    return game->current_location;
}

enum MorkResult BaseGame_save(struct Database *db, struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct GameRecord *record = Database_getGame(db, game->id);
    if (record == NULL) {
        int nextID = Database_getNextIndex(db, GAMES);

        // Find character and location records
        struct CharacterRecord *characterRecord = Database_getCharacterByName(db, game->player->name);
        if (characterRecord == NULL) {
            log_err("Failed to load character record.");
            goto error;
        }

        struct LocationRecord *locationRecord = Database_getLocationByName(db, game->current_location->name);
        if (locationRecord == NULL) {
            log_err("Failed to load location record.");
            goto error;
        }

        record = GameRecord_create(nextID, characterRecord->id, locationRecord->id);
        check(record != NULL, "Failed to create game record.");

        record->id = nextID;
        enum MorkResult res = Database_createGame(db, record);
        free(record);
        if (res != MORK_OK) {
            log_err("Failed to create game record.");

            goto error;
        }
        return MORK_OK;
    } else {
        // Game exists in DB, update it.
        // First, grab the character and location records
        struct CharacterRecord *characterRecord = Database_getCharacter(db, record->owner_id);
        if (characterRecord == NULL) {
            log_err("Failed to load character record.");
            goto error;
        }

        struct LocationRecord *locationRecord = Database_getLocation(db, record->location_id);
        if (locationRecord == NULL) {
            log_err("Failed to load location record.");
            goto error;
        }

        if (record->location_id != locationRecord->id) {
            record->location_id = locationRecord->id;
            enum MorkResult res = Database_updateGame(db, record);
            if (res != MORK_OK) {
                log_err("Failed to update game record.");
                goto error;
            }
        }
        return MORK_OK;
    }

error:
    return MORK_ERROR_DB_FAILED_SAVE;
}

struct BaseGame *BaseGame_load(struct Database *db, int id)
{
    struct GameRecord *record = Database_getGame(db, id);
    check(record != NULL, "Failed to load game.");

    struct CharacterRecord *characterRecord = Database_getCharacter(db, record->owner_id);
    check(characterRecord != NULL, "Failed to load character record.");

    struct LocationRecord *locationRecord = Database_getLocation(db, record->location_id);
    check(locationRecord != NULL, "Failed to load location record.");

    struct Character *player = Character_load(db, characterRecord->name);
    check(player != NULL, "Failed to load character.");

    struct Location *current_location = Location_load(db, locationRecord->id);
    check(current_location != NULL, "Failed to load location.");

    struct BaseGame *game = BaseGame_create(player);
    BaseGame_setLocation(game, current_location);

    return game;

error:
    return NULL;
}

struct TerminalSegment *BaseGame_move(struct Database *db, struct BaseGame *game, enum ActionTargetKind target)
{
    if (game == NULL) {
        return NULL;
    }

    struct Location *location = game->current_location;
    struct TerminalSegment *ts = TS_setNormal(TS_setWhite(TS_new()));
    // Exit mapping is [NORTH, SOUTH, EAST, WEST, UP, DOWN]

    int exit_position = -1;

    switch (target) {
        case TARGET_NORTH:
            exit_position = 1;
            break;
        case TARGET_SOUTH:
            exit_position = 2;
            break;
        case TARGET_EAST:
            exit_position = 3;
            break;
        case TARGET_WEST:
            exit_position = 4;
            break;
        case TARGET_UP:
            exit_position = 5;
            break;
        case TARGET_DOWN:
            exit_position = 6;
            break;
        default:
            break;
    }

    if (exit_position == -1) {
        return NULL;
    }

    int exit_id = location->exitIDs[exit_position];
    if (exit_id == 0) {
        TS_concatText(ts, "You can't go that way.");
        return ts;
    }

    struct Location *new_location = Location_load(db, exit_id);
    if (new_location == NULL) {
        TS_concatText(ts, "Whoa, something real weird happened. You sure that place exists?");
        return ts;
    }

    BaseGame_setLocation(game, new_location);
    TS_concatText(ts, new_location->description);
    return ts;
}

struct TerminalSegment *BaseGame_take(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetkind, char *target)
{
    if (game == NULL) {
        return NULL;
    }

    struct Location *location = game->current_location;
    struct Character *player = game->player;
    struct TerminalSegment *ts = TS_setNormal(TS_setWhite(TS_new()));

    switch (targetkind) {
        case TARGET_NONE:
        case TARGET_NORTH:
        case TARGET_SOUTH:
        case TARGET_EAST:
        case TARGET_WEST:
        case TARGET_UP:
        case TARGET_DOWN:
        case TARGET_SELF:
        case TARGET_CHARACTER:
        case TARGET_ROOM:
            break;
        case TARGET_ITEM:
            for (int i = 0; i < MAX_ITEMS; i++) {
                if (strcmp(location->items[i]->name, target) == 0) {
                    // Add item to player inventory
                    Inventory_addItem(player->inventory, location->items[i]);
                    TS_concatText(ts, "You take the ");
                    TS_concatText(TS_setBold(TS_setYellow(ts)), location->items[i]->name);
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), ".");
                    Character_save(db, player);
                    return ts;
                }
            }
            return TS_concatText(ts, "What are you trying to take?");
    }
    return TS_concatText(ts, "You seriously don't think you can take that, do you?");
}

struct TerminalSegment *BaseGame_drop(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetkind, char *target)
{
    if (game == NULL) {
        return NULL;
    }

    struct Character *player = game->player;
    struct TerminalSegment *ts = TS_setNormal(TS_setWhite(TS_new()));

    switch (targetkind) {
        case TARGET_NONE:
        case TARGET_NORTH:
        case TARGET_SOUTH:
        case TARGET_EAST:
        case TARGET_WEST:
        case TARGET_UP:
        case TARGET_DOWN:
        case TARGET_SELF:
        case TARGET_CHARACTER:
        case TARGET_ROOM:
            break;
        case TARGET_ITEM:
            for (int i = 0; i < MAX_ITEMS; i++) {
                if (strcmp(player->inventory->items[i]->name, target) == 0) {
                    // Remove item from player inventory
                    Inventory_removeItem(player->inventory, player->inventory->items[i]);
                    TS_concatText(ts, "You drop the ");
                    TS_concatText(TS_setBold(TS_setYellow(ts)), player->inventory->items[i]->name);
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), ".");

                    Character_save(db, player);
                    return ts;
                }
            }
            return TS_concatText(ts, "I don't think you're holding one of those.");
    }   
    return TS_concatText(ts, "What are you talking about? You sound crazy right now.");
}

struct TerminalSegment *BaseGame_look(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetKind, char *target)
{
    if (game == NULL) {
        return NULL;
    }

    struct Location *location = game->current_location;
    struct TerminalSegment *ts = TS_new();

    switch (targetKind) {
        case TARGET_NONE:
            break;
        case TARGET_NORTH:
            // Look at the north exit
            if (location->exitIDs[0] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[0]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
            }
            return ts;
        case TARGET_SOUTH:
            // Look at the south exit
            if (location->exitIDs[1] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[1]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
            }
            return ts;
        case TARGET_EAST:
            // Look at the east exit
            if (location->exitIDs[2] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[2]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
            }
            return ts;
        case TARGET_WEST:
            // Look at the west exit
            if (location->exitIDs[3] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[3]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing over there.");
            }
            return ts;
        case TARGET_UP:
            // Look at the up exit
            if (location->exitIDs[4] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[4]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing up there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing up there.");
            }
            return ts;
        case TARGET_DOWN:
            // Look at the down exit
            if (location->exitIDs[5] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[5]);
                if (new_location != NULL) {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), new_location->description);
                } else {
                    TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing down there.");
                }
            } else {
                TS_concatText(TS_setNormal(TS_setWhite(ts)), "There's nothing down there.");
            }
            return ts;
        case TARGET_ITEM:
            for (int i = 0; i < MAX_ITEMS; i++) {
                if (strcmp(location->items[i]->name, target) == 0) {
                    return TS_concatText(TS_setNormal(TS_setWhite(ts)), location->items[i]->description);
                }
            }
            return TS_concatText(TS_setNormal(TS_setWhite(ts)), "What are you looking at?");
        case TARGET_SELF:
            // Look at the player
            TS_concatText(TS_setNormal(TS_setWhite(ts)), "You are ");
            TS_concatText(TS_setBold(TS_setYellow(ts)), game->player->name);
            TS_concatText(TS_setNormal(TS_setWhite(ts)), ".");
            return ts;
        case TARGET_CHARACTER:
            break;
        case TARGET_ROOM:
            // Look at the current room
            TS_concatText(TS_setNormal(TS_setWhite(ts)), location->description);
            return ts;
    }

    return TS_concatText(TS_setNormal(TS_setWhite(ts)), "You look around, but see nothing of interest.");
}

struct TerminalSegment *BaseGame_inventory(struct BaseGame *game)
{
    if (game == NULL) {
        return NULL;
    }

    struct TerminalSegment *ts = TS_new();
    check(ts != NULL, "Failed to create terminal segment.");

    TS_concatText(TS_setNormal(TS_setWhite(ts)), "Inventory:\n");

    struct Character *player = game->player;

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (player->inventory->items[i] != NULL) {
            TS_concatText(TS_setYellow(TS_setBold(ts)), player->inventory->items[i]->name);
            TS_concatText(TS_setNormal(TS_setWhite(ts)), "\n");
        }
    }
    return ts;

error:
    return NULL;
}

struct TerminalSegment *BaseGame_help(struct BaseGame *game)
{
    if (game == NULL) {
        return NULL;
    }

    struct TerminalSegment *frame = TS_new();
    check(frame != NULL, "Failed to create frame.");

    TS_concatText(TS_setNormal(TS_setWhite(frame)), "Commands:\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "look [north, south, east, west, up, down, room, item, self, character]\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "move [north, south, east, west, up, down]\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "take [item]\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "drop [item]\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "inventory\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "help\n");
    TS_concatText(TS_setBold(TS_setWhite(frame)), "quit\n");

    return frame;

error:
    return NULL;
}

void BaseGame_quit(struct Database *db, struct BaseGame *game)
{
    if (db != NULL) Database_close(db);
    if (game != NULL) BaseGame_destroy(game);

    exit(0);
}

struct TerminalSegment *BaseGame_execute(struct Database *db, struct BaseGame *game, struct Action *action)
{
    if (game == NULL) {
        return NULL;
    }
    if (action == NULL) {
        return NULL;
    }

    struct TerminalSegment *frame = TS_new();
    check(frame != NULL, "Failed to create frame.");

    // Add context to the body frame
    TS_concatText(TS_setNormal(TS_setWhite(frame)), "You are in ");
    TS_concatText(TS_setBold(frame), game->current_location->name);

    // Execute the action
    switch (action->kind) {
        case ACTION_MOVE:
            TS_append(frame, BaseGame_move(db, game, action->target_kind));
            break;
        case ACTION_TAKE:
            TS_append(frame, BaseGame_take(db, game, action->target_kind, action->noun));
            break;
        case ACTION_DROP:
            TS_append(frame, BaseGame_drop(db, game, action->target_kind, action->noun));
            break;
        case ACTION_LOOK:
            TS_append(frame, BaseGame_look(db, game, action->target_kind, action->noun));
            break;
        case ACTION_INVENTORY:
            TS_append(frame, BaseGame_inventory(game));
            break;
        case ACTION_HELP:
            TS_append(frame, BaseGame_help(game));
            break;
        case ACTION_QUIT:
            BaseGame_quit(db, game);
            break;
        default:
            break;
    }

    return frame;

error:
    return NULL;
}

enum MorkResult BaseGame_run(struct Database *db, struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    // Run the game loop
    while (1) {
        BaseGame_refreshScreen(game);

        // Read and parse player input
        char *input = NULL;
        size_t len = 0;
        ssize_t read = getline(&input, &len, stdin);
        if (read == -1) {
            log_err("Failed to read input.");
            return MORK_ERROR_MODEL_GAME_INPUT;
        }

        // Parse the input
        struct Action *action = Action_create(input);
        if (action == NULL) {
            log_err("Failed to create action.");
            return MORK_ERROR_MODEL_ACTION_NULL;
        }
        enum MorkResult res = Action_parse(action);
        if (res != MORK_OK) {
            log_err("Failed to parse action, error code: %d", res);
            Action_destroy(action);
            continue;
        }

        // Execute the action
        res = BaseGame_executeAction(db, game, action);
        if (res != MORK_OK) {
            log_err("Action: %s", input);
            log_err("Failed to execute action, error code: %d", res);
            return res;
        }

        // Check for game over
        if (game->player->health <= 0) {
            log_info("Game over.");
            break;
        }

        // Cleanup
        Action_destroy(action);
    }

    return MORK_OK;
}

char *BaseGame_getScreenDisplay(struct BaseGame *game)
{
    if (game == NULL) {
        return NULL;
    }

    return ScreenState_getDisplay(game->screen);
}