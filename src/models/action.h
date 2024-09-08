#pragma once

#include "../utils/error.h"

#include <stdlib.h>

#define MAX_INPUT 255

enum ActionKind {
    ACTION_NONE,
    ACTION_MOVE,
    ACTION_LOOK,
    ACTION_TAKE,
    ACTION_DROP,
    ACTION_INVENTORY,
    ACTION_HELP,
    ACTION_QUIT
};

enum ActionTargetKind {
    TARGET_NONE,
    TARGET_NORTH,
    TARGET_SOUTH,
    TARGET_EAST,
    TARGET_WEST,
    TARGET_UP,
    TARGET_DOWN,
    TARGET_ITEM,
    TARGET_SELF,
    TARGET_CHARACTER,
    TARGET_ROOM,
    TARGET_PROP
};

struct Action {
    char raw_input[MAX_INPUT];
    char verb[MAX_INPUT];
    char noun[MAX_INPUT];
    enum ActionKind kind;
    enum ActionTargetKind target_kind;
    int target_id;
};

struct Action *Action_create(const char *input);
enum MorkResult Action_destroy(struct Action *action);

enum MorkResult Action_parse(struct Action *action);
