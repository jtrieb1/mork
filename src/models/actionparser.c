#include "actionparser.h"
#include "../coredb/tables/character.h"
#include "../coredb/tables/items.h"
#include "../utils/error.h"

static struct tagbstring move_cmd = bsStatic("move");
static struct tagbstring look_cmd = bsStatic("look");
static struct tagbstring take_cmd = bsStatic("take");
static struct tagbstring drop_cmd = bsStatic("drop");
static struct tagbstring inventory_cmd = bsStatic("inventory");
static struct tagbstring help_cmd = bsStatic("help");
struct tagbstring quit_cmd = bsStatic("quit");

static struct tagbstring north_target = bsStatic("north");
static enum ActionTargetKind north_kind = TARGET_NORTH;
static struct tagbstring south_target = bsStatic("south");
static enum ActionTargetKind south_kind = TARGET_SOUTH;
static struct tagbstring east_target = bsStatic("east");
static enum ActionTargetKind east_kind = TARGET_EAST;
static struct tagbstring west_target = bsStatic("west");
static enum ActionTargetKind west_kind = TARGET_WEST;
static struct tagbstring up_target = bsStatic("up");
static enum ActionTargetKind up_kind = TARGET_UP;
static struct tagbstring down_target = bsStatic("down");
static enum ActionTargetKind down_kind = TARGET_DOWN;
static struct tagbstring room_target = bsStatic("room");
static enum ActionTargetKind room_kind = TARGET_ROOM;
static struct tagbstring self_target = bsStatic("self");
static enum ActionTargetKind self_kind = TARGET_SELF;

enum ActionKind *ActionKind_allocd(enum ActionKind kind)
{
    enum ActionKind *kindPtr = malloc(sizeof(enum ActionKind));
    check_mem(kindPtr);

    *kindPtr = kind;

    return kindPtr;

error:
    return NULL;
}

enum ActionTargetKind *ActionTargetKind_allocd(enum ActionTargetKind kind)
{
    enum ActionTargetKind *kindPtr = malloc(sizeof(enum ActionTargetKind));
    check_mem(kindPtr);

    *kindPtr = kind;

    return kindPtr;

error:
    return NULL;
}

ActionParser *ActionParser_create(struct Database *db)
{
    ActionParser *parser = malloc(sizeof(ActionParser));
    check_mem(parser);

    parser->verbs = Hashmap_create(NULL, NULL);
    check_mem(parser->verbs);

    parser->input = NULL;
    parser->verb = NULL;
    parser->noun = NULL;
    parser->db = db;

    ActionParser_preload(parser);

    return parser;

error:
    if (parser) {
        free(parser);
    }

    return NULL;
}

ActionVerbEntry *ActionVerbEntry_create(enum ActionKind kind)
{
    ActionVerbEntry *verbEntry = malloc(sizeof(ActionVerbEntry));
    check_mem(verbEntry);

    verbEntry->kind = kind;
    verbEntry->nouns = Hashmap_create(NULL, NULL);
    check_mem(verbEntry->nouns);

    return verbEntry;

error:
    if (verbEntry) {
        free(verbEntry);
    }

    return NULL;
}

void ActionVerbEntry_destroy(ActionVerbEntry *verbEntry)
{
    if (verbEntry) {
        if (verbEntry->nouns) {
            // Destroy the nouns hashmap
            Hashmap_destroy(verbEntry->nouns);
            verbEntry->nouns = NULL;
        }

        free(verbEntry);
    }
}

void ActionParser_preload(ActionParser *parser)
{
    check(parser, "ActionParser is NULL");
    
    // Load verbs
    Hashmap_set(parser->verbs, &move_cmd, ActionVerbEntry_create(ACTION_MOVE));
    Hashmap_set(parser->verbs, &look_cmd, ActionVerbEntry_create(ACTION_LOOK));
    Hashmap_set(parser->verbs, &take_cmd, ActionVerbEntry_create(ACTION_TAKE));
    Hashmap_set(parser->verbs, &drop_cmd, ActionVerbEntry_create(ACTION_DROP));
    Hashmap_set(parser->verbs, &inventory_cmd, ActionVerbEntry_create(ACTION_INVENTORY));
    Hashmap_set(parser->verbs, &help_cmd, ActionVerbEntry_create(ACTION_HELP));
    Hashmap_set(parser->verbs, &quit_cmd, ActionVerbEntry_create(ACTION_QUIT));

    // Load nouns
    ActionVerbEntry *move = Hashmap_get(parser->verbs, &move_cmd);
    Hashmap_set(move->nouns, &north_target, &north_kind);
    Hashmap_set(move->nouns, &south_target, &south_kind);
    Hashmap_set(move->nouns, &east_target, &east_kind);
    Hashmap_set(move->nouns, &west_target, &west_kind);
    Hashmap_set(move->nouns, &up_target, &up_kind);
    Hashmap_set(move->nouns, &down_target, &down_kind);
    
    ActionVerbEntry *look = Hashmap_get(parser->verbs, &look_cmd);
    Hashmap_set(look->nouns, &north_target, &north_kind);
    Hashmap_set(look->nouns, &south_target, &south_kind);
    Hashmap_set(look->nouns, &east_target, &east_kind);
    Hashmap_set(look->nouns, &west_target, &west_kind);
    Hashmap_set(look->nouns, &up_target, &up_kind);
    Hashmap_set(look->nouns, &down_target, &down_kind);
    Hashmap_set(look->nouns, &room_target, &room_kind);
    Hashmap_set(look->nouns, &self_target, &self_kind);

    return;

error:
    return;
}

int hashmap_cleanup_verbs(HashmapNode *node)
{
    if (node->data) {
        ActionVerbEntry_destroy(node->data);
    }

    return 0;
}

void ActionParser_destroy(ActionParser *parser)
{
    if (parser) {
        if (parser->verbs) {
            // First, go through and free the associated hashmaps
            Hashmap_traverse(parser->verbs, hashmap_cleanup_verbs);
            Hashmap_destroy(parser->verbs);
            parser->verbs = NULL;
        }

        if (parser->input) {
            bdestroy(parser->input);
            parser->input = NULL;
        }

        if (parser->verb) {
            bdestroy(parser->verb);
            parser->verb = NULL;
        }

        if (parser->noun) {
            bdestroy(parser->noun);
            parser->noun = NULL;
        }

        free(parser);
    }
}

enum MorkResult ActionParser_load(ActionParser *parser, bstring raw_input) {
    check(parser, "ActionParser is NULL");
    check(raw_input, "raw_input is NULL");

    debug("Raw input: %s", bdata(raw_input));

    btrimws(raw_input);
    debug("Trimmed input: %s", bdata(raw_input));

    parser->input = bstrcpy(raw_input);

    debug("Input: %s", bdata(parser->input));

    // Split input into verb and noun if possible
    int i = 0;
    for (i = 0; i < blength(raw_input); i++) {
        if (bchar(raw_input, i) == ' ') {
            break;
        }
    }

    if (i == 0) {
        // No verb found
        return MORK_ERROR_MODEL_ACTION_PARSE_VERB;
    }
    if (i == blength(raw_input) - 1) {
        // No noun found
        parser->verb = bstrcpy(raw_input);
        debug("Verb: %s", bdata(parser->verb));
        return MORK_OK;
    }

    parser->verb = bmidstr(raw_input, 0, i);
    parser->noun = bmidstr(raw_input, i + 1, blength(raw_input) - i - 1);
    debug("Verb: %s", bdata(parser->verb));
    debug("Noun: %s", bdata(parser->noun));

    return MORK_OK;

error:
    return MORK_ERROR_MODEL_ACTION_PARSE;
}

unsigned int ActionParser_isLoaded(ActionParser *parser)
{
    return parser->input != NULL;
}

void ActionParser_reset(ActionParser *parser)
{
    if (parser->input) {
        bdestroy(parser->input);
        parser->input = NULL;
    }

    if (parser->verb) {
        bdestroy(parser->verb);
        parser->verb = NULL;
    }

    if (parser->noun) {
        bdestroy(parser->noun);
        parser->noun = NULL;
    }

    if (parser->verbs) {
        Hashmap_destroy(parser->verbs);
        parser->verbs = NULL;
    }
}

enum MorkResult ActionParser_parse(ActionParser *parser, enum ActionKind *kind, enum ActionTargetKind *targetKind)
{
    check(parser, "ActionParser is NULL");
    check(kind, "kind is NULL");
    check(targetKind, "targetKind is NULL");

    if (blength(parser->input) == 0 || blength(parser->verb) == 0) {
        debug("Input or verb is empty");
        return MORK_ERROR_MODEL_ACTION_PARSE;
    }

    // Find verb in hashmap
    ActionVerbEntry *verbEntry = Hashmap_get(parser->verbs, parser->verb);
    check(verbEntry, "Verb not found");

    // Check if verb should have a noun
    if (verbEntry->kind == ACTION_INVENTORY || verbEntry->kind == ACTION_HELP || verbEntry->kind == ACTION_QUIT) {
        *kind = verbEntry->kind;
        *targetKind = TARGET_NONE;

        return MORK_OK;
    }

    // Find noun in hashmap
    enum ActionTargetKind *nounEntry = Hashmap_get(verbEntry->nouns, parser->noun);
    // If no noun is found, it could still be an item or character
    if (!nounEntry) {
        // Check if noun is an item
        struct ItemRecord *item = Database_getItemByName(parser->db, bdata(parser->noun));
        if (item) {
            *kind = verbEntry->kind;
            *targetKind = TARGET_ITEM;

            return MORK_OK;
        }

        // Check if noun is a character
        struct CharacterRecord *character = Database_getCharacterByName(parser->db, bdata(parser->noun));
        if (character) {
            *kind = verbEntry->kind;
            *targetKind = TARGET_CHARACTER;

            return MORK_OK;
        }

        return MORK_ERROR_MODEL_ACTION_PARSE;
    }

    *kind = verbEntry->kind;
    *targetKind = *nounEntry;

    bdestroy(parser->input);
    parser->input = NULL;
    bdestroy(parser->verb);
    parser->verb = NULL;

    return MORK_OK;

error:
    return MORK_ERROR_MODEL_ACTION_PARSE;
}
