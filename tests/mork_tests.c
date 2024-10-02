#include "minunit.h"

#include "../src/coredb/db.h"
#include "../src/models/game.h"

char *test_create()
{
    struct Database *db = Database_create();
    mu_assert(db != NULL, "Failed to create database.");

    Database_open(db, "tests/db/test.db");

    struct Character *player = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    mu_assert(player != NULL, "Failed to create character.");

    struct BaseGame *game = BaseGame_create(player);
    mu_assert(game != NULL, "Failed to create game.");
    mu_assert(game->player != NULL, "Failed to set player.");
    mu_assert(game->current_location == NULL, "Failed to set current_location.");

    BaseGame_destroy(game);
    Database_destroy(db);
    return NULL;
}

char *test_functions()
{
    return NULL;
}

char *test_failures()
{
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_functions);
    mu_run_test(test_failures);

    return NULL;
}

RUN_TESTS(all_tests);
