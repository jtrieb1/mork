#include "action.h"

#include <lcthw/dbg.h>
#include <ctype.h>

struct Action *Action_create(const char *input)
{
    struct Action *action = malloc(sizeof(struct Action));
    check_mem(action);

    strncpy(action->raw_input, input, MAX_INPUT);
    action->raw_input[MAX_INPUT - 1] = '\0';

    memset(action->result, 0, MAX_OUTPUT);

    return action;

error:
    return NULL;
}

enum MorkResult Action_destroy(struct Action *action)
{
    if (action == NULL) {
        return MORK_ERROR_MODEL_ACTION_NULL;
    }
    free(action);
    return MORK_OK;
}

enum ActionTargetKind Action_getTargetKind(struct Action *action, struct Database *db)
{
    if (action == NULL) {
        return TARGET_NONE;
    }

    if (strstr(action->noun, "north") != NULL) {
        return TARGET_NORTH;
    } else if (strstr(action->noun, "south") != NULL) {
        return TARGET_SOUTH;
    } else if (strstr(action->noun, "east") != NULL) {
        return TARGET_EAST;
    } else if (strstr(action->noun, "west") != NULL) {
        return TARGET_WEST;
    } else if (strstr(action->noun, "up") != NULL) {
        return TARGET_UP;
    } else if (strstr(action->noun, "down") != NULL) {
        return TARGET_DOWN;
    } else if (strstr(action->noun, "self") != NULL) {
        return TARGET_SELF;
    } else {
        // Check for item
        struct ItemRecord *item = Database_getItemByName(db, action->noun);
        if (item != NULL) {
            return TARGET_ITEM;
        }

        // Check for character
        struct CharacterRecord *character = Database_getCharacterByName(db, action->noun);
        if (character != NULL) {
            return TARGET_CHARACTER;
        }

        return TARGET_NONE;
    }
}

enum MorkResult Action_parse(struct Action *action, struct Database *db)
{
    // Expect all inputs to be either "verb noun" or just "verb"
    char *verb = strtok(action->raw_input, " ");
    if (verb == NULL) {
        return MORK_ERROR_MODEL_ACTION_PARSE_VERB;
    }

    strncpy(action->verb, verb, MAX_INPUT);
    action->verb[MAX_INPUT - 1] = '\0';

    // There may or may not be a noun, check the verb to see if it contains the words
    // 'inventory', 'help', or 'quit'
    if (strstr(action->verb, "inventory") != NULL) {
        action->kind = ACTION_INVENTORY;
        action->target_kind = TARGET_NONE;
        return MORK_OK;
    } else if (strstr(action->verb, "help") != NULL) {
        action->kind = ACTION_HELP;
        action->target_kind = TARGET_NONE;
        return MORK_OK;
    } else if (strstr(action->verb, "quit") != NULL) {
        action->kind = ACTION_QUIT;
        action->target_kind = TARGET_NONE;
        return MORK_OK;
    }

    // Otherwise, deal with the noun
    char *noun = strtok(NULL, " ");
    if (noun == NULL) {
        action->kind = ACTION_NONE;
        action->target_kind = TARGET_NONE;
        return MORK_ERROR_MODEL_ACTION_PARSE_NOUN;
    }

    // Strip any leading or trailing whitespace
    while (isspace(*noun)) {
        noun++;
    }
    char *end = noun + strlen(noun) - 1;
    while (end > noun && isspace(*end)) {
        end--;
    }
    end[1] = '\0';

    strcpy(action->noun, noun);

    // Check for movement
    if (strstr(action->verb, "move") != NULL) {
        action->kind = ACTION_MOVE;
        action->target_kind = Action_getTargetKind(action, db);
        return MORK_OK;
    }

    // Check for look
    if (strstr(action->verb, "look") != NULL) {
        action->kind = ACTION_LOOK;
        action->target_kind = Action_getTargetKind(action, db);
        return MORK_OK;
    }

    // Check for take
    if (strstr(action->verb, "take") != NULL) {
        action->kind = ACTION_TAKE;
        action->target_kind = TARGET_ITEM;
        return MORK_OK;
    }

    // Check for drop
    if (strstr(action->verb, "drop") != NULL) {
        action->kind = ACTION_DROP;
        action->target_kind = TARGET_ITEM;
        return MORK_OK;
    }

    return MORK_ERROR_MODEL_ACTION_PARSE_VERB;
}