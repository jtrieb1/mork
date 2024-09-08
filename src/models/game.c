#include "game.h"

#include <lcthw/dbg.h>
#include <sys/types.h>

struct BaseGame *BaseGame_create(struct Character *player)
{

    struct BaseGame *game = calloc(1, sizeof(struct BaseGame));
    check_mem(game);

    game->player = player;
    game->current_location = NULL;

    for (int i = 0; i < MAX_HISTORY; i++) {
        game->history[i] = NULL;
    }

    return game;

error:
    return NULL;
}

enum MorkResult BaseGame_destroy(struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    free(game);
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

enum MorkResult BaseGame_move(struct Database *db, struct BaseGame *game, enum ActionTargetKind target)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct Location *location = game->current_location;
    // Exit mapping is [NORTH, SOUTH, EAST, WEST, UP, DOWN]

    switch (target) {
        case TARGET_NORTH:
            if (location->exitIDs[0] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[0]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        case TARGET_SOUTH:
            if (location->exitIDs[1] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[1]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        case TARGET_EAST:
            if (location->exitIDs[2] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[2]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        case TARGET_WEST:
            if (location->exitIDs[3] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[3]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        case TARGET_UP:
            if (location->exitIDs[4] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[4]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        case TARGET_DOWN:
            if (location->exitIDs[5] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[5]);
                if (new_location != NULL) {
                    BaseGame_setLocation(game, new_location);
                }
            }
            break;
        default:
            break;
    }

    return MORK_OK;
}

enum MorkResult BaseGame_take(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetkind, char *target)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct Location *location = game->current_location;
    struct Character *player = game->player;

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
                if (strcmp(location->items[i].name, target) == 0) {
                    // Add item to player inventory
                    Inventory_addItem(player->inventory, &location->items[i]);
                }
            }
            break;
    }
    return Character_save(db, player) == -1 ? MORK_ERROR_DB_FAILED_SAVE : MORK_OK;
}

enum MorkResult BaseGame_drop(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetkind, char *target)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct Character *player = game->player;

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
                }
            }
            break;
    }   
    return Character_save(db, player) == -1 ? MORK_ERROR_DB_FAILED_SAVE : MORK_OK;
}

enum MorkResult BaseGame_look(struct Database *db, struct BaseGame *game, enum ActionTargetKind targetKind, char *target)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct Location *location = game->current_location;

    switch (targetKind) {
        case TARGET_NONE:
            break;
        case TARGET_NORTH:
            // Look at the north exit
            if (location->exitIDs[0] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[0]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_SOUTH:
            // Look at the south exit
            if (location->exitIDs[1] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[1]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_EAST:
            // Look at the east exit
            if (location->exitIDs[2] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[2]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_WEST:
            // Look at the west exit
            if (location->exitIDs[3] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[3]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_UP:
            // Look at the up exit
            if (location->exitIDs[4] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[4]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_DOWN:
            // Look at the down exit
            if (location->exitIDs[5] != 0) {
                struct Location *new_location = Location_load(db, location->exitIDs[5]);
                if (new_location != NULL) {
                    printf("%s\n", new_location->description);
                }
            }
            break;
        case TARGET_ITEM:
            for (int i = 0; i < MAX_ITEMS; i++) {
                if (strcmp(location->items[i].name, target) == 0) {
                    printf("%s\n", location->items[i].description);
                }
            }
            break;
        case TARGET_SELF:
            break;
        case TARGET_CHARACTER:
            break;
        case TARGET_ROOM:
            // Look at the current room
            printf("%s\n", location->description);
            break;
    }

    return MORK_OK;
}

enum MorkResult BaseGame_inventory(struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    struct Character *player = game->player;

    for (int i = 0; i < MAX_ITEMS; i++) {
        if (player->inventory->items[i] != NULL) {
            printf("%s\n", player->inventory->items[i]->name);
        }
    }

    return MORK_OK;
}

enum MorkResult BaseGame_help(struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    printf("Available commands:\n");
    printf("move [north, south, east, west, up, down]\n");
    printf("look [north, south, east, west, up, down, item, self, character, room]\n");
    printf("take [item]\n");
    printf("drop [item]\n");
    printf("inventory\n");
    printf("help\n");
    printf("quit\n");

    return MORK_OK;
}

void BaseGame_quit(struct Database *db, struct BaseGame *game)
{
    if (db != NULL) Database_close(db);
    if (game != NULL) BaseGame_destroy(game);

    exit(0);
}

enum MorkResult BaseGame_execute(struct Database *db, struct BaseGame *game, struct Action *action)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }
    if (action == NULL) {
        return MORK_ERROR_MODEL_ACTION_NULL;
    }

    // Execute the action
    switch (action->kind) {
        case ACTION_MOVE:
            return BaseGame_move(db, game, action->target_kind);
        case ACTION_TAKE:
            return BaseGame_take(db, game, action->target_kind, action->noun);
        case ACTION_DROP:
            return BaseGame_drop(db, game, action->target_kind, action->noun);
        case ACTION_LOOK:
            return BaseGame_look(db, game, action->target_kind, action->noun);
        case ACTION_INVENTORY:
            return BaseGame_inventory(game);
        case ACTION_HELP:
            return BaseGame_help(game);
        case ACTION_QUIT:
            BaseGame_quit(db, game);
        default:
            break;
    }

    // Add action to history
    for (int i = 0; i < MAX_HISTORY; i++) {
        if (game->history[i] == NULL) {
            game->history[i] = action;
            break;
        }
    }

    return MORK_OK;
}

enum MorkResult BaseGame_run(struct Database *db, struct BaseGame *game)
{
    if (game == NULL) {
        return MORK_ERROR_MODEL_GAME_NULL;
    }

    // Run the game loop
    while (1) {
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
            log_err("Failed to parse action.");
            return res;
        }

        // Execute the action
        res = BaseGame_execute(db, game, action);
        if (res != MORK_OK) {
            log_err("Failed to execute action.");
            return res;
        }

        // Check for game over
        if (game->player->health <= 0) {
            log_info("Game over.");
            break;
        }

        // Cleanup
        free(input);
        Action_destroy(action);
    }

    return MORK_OK;
}
