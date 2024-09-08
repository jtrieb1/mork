#include "game.h"

#include <lcthw/dbg.h>

struct BaseGame *BaseGame_create(struct Character *player)
{

    struct BaseGame *game = calloc(1, sizeof(struct BaseGame));
    check_mem(game);

    game->player = player;
    game->current_location = NULL;

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