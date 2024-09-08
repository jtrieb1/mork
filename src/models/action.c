#include "action.h"

#include <lcthw/dbg.h>

struct Action *Action_create(const char *input)
{
    struct Action *action = malloc(sizeof(struct Action));
    check_mem(action);

    strncpy(action->raw_input, input, MAX_INPUT);
    action->raw_input[MAX_INPUT - 1] = '\0';

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
    if (action == NULL) {
        return MORK_ERROR_MODEL_ACTION_NULL;
    }

    char *token = strtok(action->raw_input, " ");
    if (token == NULL) {
        action->kind = ACTION_NONE;
        return MORK_OK;
    }

    strncpy(action->verb, token, MAX_INPUT);
    action->verb[MAX_INPUT - 1] = '\0';

    token = strtok(NULL, " ");
    if (token == NULL) {
        action->kind = ACTION_NONE;
        return MORK_OK;
    }

    strncpy(action->noun, token, MAX_INPUT);
    action->noun[MAX_INPUT - 1] = '\0';

    if (strcmp(action->verb, "move") == 0) {
        action->kind = ACTION_MOVE;
    } else if (strcmp(action->verb, "look") == 0) {
        action->kind = ACTION_LOOK;
    } else if (strcmp(action->verb, "take") == 0) {
        action->kind = ACTION_TAKE;
    } else if (strcmp(action->verb, "drop") == 0) {
        action->kind = ACTION_DROP;
    } else if (strcmp(action->verb, "inventory") == 0) {
        action->kind = ACTION_INVENTORY;
    } else if (strcmp(action->verb, "help") == 0) {
        action->kind = ACTION_HELP;
    } else if (strcmp(action->verb, "quit") == 0) {
        action->kind = ACTION_QUIT;
    } else {
        action->kind = ACTION_NONE;
    }

    action->target_kind = Action_getTargetKind(action);

    return MORK_OK;
}