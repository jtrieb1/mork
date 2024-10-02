#include "action.h"

#include <lcthw/dbg.h>

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

enum ActionTargetKind Action_getTargetKind(struct Action *action)
{
    if (action == NULL) {
        return TARGET_NONE;
    }

    if (strcmp(action->noun, "north") == 0) {
        return TARGET_NORTH;
    } else if (strcmp(action->noun, "south") == 0) {
        return TARGET_SOUTH;
    } else if (strcmp(action->noun, "east") == 0) {
        return TARGET_EAST;
    } else if (strcmp(action->noun, "west") == 0) {
        return TARGET_WEST;
    } else if (strcmp(action->noun, "up") == 0) {
        return TARGET_UP;
    } else if (strcmp(action->noun, "down") == 0) {
        return TARGET_DOWN;
    } else {
        return TARGET_NONE;
    }
}

enum MorkResult Action_parse(struct Action *action)
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

    strncpy(action->noun, noun, MAX_INPUT);
    action->noun[MAX_INPUT - 1] = '\0';

    // Check for movement
    if (strstr(action->verb, "move") != NULL) {
        action->kind = ACTION_MOVE;
        action->target_kind = Action_getTargetKind(action);
        return MORK_OK;
    }

    // Check for look
    if (strstr(action->verb, "look") != NULL) {
        action->kind = ACTION_LOOK;
        action->target_kind = Action_getTargetKind(action);
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