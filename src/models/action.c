#include "action.h"

#include <lcthw/dbg.h>
#include <ctype.h>

struct Action *Action_create(const char *input)
{
    struct Action *action = malloc(sizeof(struct Action));
    check_mem(action);

    action->raw_input = bfromcstr(input);
    action->noun = NULL;

    action->kind = ACTION_NONE;
    action->target_kind = TARGET_NONE;

    action->parser = NULL;

    return action;

error:
    return NULL;
}

enum MorkResult Action_destroy(struct Action *action)
{
    if (action == NULL) {
        return MORK_ERROR_MODEL_ACTION_NULL;
    }
    if (action->parser != NULL) {
        ActionParser_destroy(action->parser);
    }
    if (action->noun != NULL) {
        bdestroy(action->noun);
    }
    if (action->raw_input != NULL) {
        bdestroy(action->raw_input);
    }
    free(action);
    return MORK_OK;
}

enum MorkResult Action_parse(struct Action *action, struct Database *db)
{
    if (action == NULL) {
        return MORK_ERROR_MODEL_ACTION_NULL;
    }

    action->parser = ActionParser_create(db);
    check(action->parser != NULL, "Failed to create action parser.");
    ActionParser_preload(action->parser);

    if (ActionParser_load(action->parser, action->raw_input) != MORK_OK) {
        return MORK_ERROR_MODEL_ACTION_PARSE;
    }

    if (ActionParser_isLoaded(action->parser) == 0) {
        return MORK_ERROR_MODEL_ACTION_PARSE;
    }

    enum MorkResult res = ActionParser_parse(action->parser, &action->kind, &action->target_kind);
    log_err("Result of parsing action: %d", res);
    check(res == MORK_OK, "Failed to parse action.");

    if (action->noun != NULL) {
        bdestroy(action->noun);
    }
    action->noun = bstrcpy(action->parser->noun);
    
    ActionParser_destroy(action->parser);
    action->parser = NULL;

    return MORK_OK;

error:
    return MORK_ERROR_MODEL_ACTION_PARSE;
}