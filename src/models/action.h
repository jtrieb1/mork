#pragma once

#include "../coredb/db.h"
#include "../utils/error.h"

#include "actionparser.h"

#include <lcthw/bstrlib.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 255
#define MAX_OUTPUT 2048

struct Action {
    bstring raw_input;
    bstring noun;
    enum ActionKind kind;
    enum ActionTargetKind target_kind;
    int target_id;
    ActionParser *parser;
};

struct Action *Action_create(const char *input);
enum MorkResult Action_destroy(struct Action *action);

enum MorkResult Action_parse(struct Action *action, struct Database *db);
