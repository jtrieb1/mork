#pragma once

#include <lcthw/bstrlib.h>
#include <lcthw/hashmap.h>

#include "../coredb/db.h"
#include "../utils/error.h"

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

enum ActionKind *ActionKind_allocd(enum ActionKind kind);

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
};

enum ActionTargetKind *ActionTargetKind_allocd(enum ActionTargetKind kind);

typedef struct ActionParser {
    Hashmap *verbs;
    bstring input;
    bstring verb;
    bstring noun;

    // Not owned
    struct Database *db;
} ActionParser;

typedef struct ActionVerbEntry {
    enum ActionKind kind;
    Hashmap *nouns;
} ActionVerbEntry;

ActionVerbEntry *ActionVerbEntry_create(enum ActionKind kind);
void ActionVerbEntry_destroy(ActionVerbEntry *verbEntry);

ActionParser *ActionParser_create(struct Database *db);
void ActionParser_preload(ActionParser *parser);
void ActionParser_destroy(ActionParser *parser);

enum MorkResult ActionParser_load(ActionParser *parser, bstring raw_input);
unsigned int ActionParser_isLoaded(ActionParser *parser);

void ActionParser_reset(ActionParser *parser);

enum MorkResult ActionParser_parse(ActionParser *parser, enum ActionKind *kind, enum ActionTargetKind *targetKind);
char *ActionParser_getNoun(ActionParser *parser);