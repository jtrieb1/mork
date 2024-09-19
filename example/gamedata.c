#include "gamedata.h"

#include <mork/models/character.h>
#include <mork/models/location.h>
#include <mork/models/item.h>

void generate_characters(struct Database *db)
{
    struct Character *mork = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct Character *mindy = Character_create(
        "Mindy",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct Character *frank = Character_create(
        "Frank",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct Character *earl = Character_create(
        "Earl",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    Character_save(db, mork);
    Character_save(db, mindy);
    Character_save(db, frank);
    Character_save(db, earl);

    Character_destroy(mork);
    Character_destroy(mindy);
    Character_destroy(frank);
    Character_destroy(earl);
}

void generate_items(struct Database *db)
{
    struct Item *item = Item_create("Item", "A generic item");
    Item_save(db, item);
    Item_destroy(item);
}

void generate_locations(struct Database *db)
{
    struct Location *start = Location_create("Your Apartment", "A small apartment mostly full of boxes");
    struct Location *mnms = Location_create("Mork and Mindy's Apt.", "A small place shared by Mork and Mindy");
    struct Location *diner = Location_create("The Diner", "A small diner with a jukebox");
    struct Location *forest = Location_create("The Forest", "A dark and spooky forest");
    struct Location *pool = Location_create("The Pool", "A cool aboveground pool");
    struct Location *spaceship = Location_create("The Spaceship", "A spaceship with a lot of buttons");
    struct Location *ork = Location_create("Ork", "The planet Ork, home of Mork");

    // Save to generate IDs
    Location_save(db, start);
    Location_save(db, mnms);
    Location_save(db, diner);
    Location_save(db, forest);
    Location_save(db, pool);
    Location_save(db, spaceship);
    Location_save(db, ork);

    // Add exits
    Location_addExit(start, NORTH, mnms->id);
    Location_addExit(mnms, SOUTH, start->id);
    Location_save(db, start);
    Location_save(db, mnms);

    // Destroy until needed
    Location_destroy(start);
    Location_destroy(mnms);
    Location_destroy(diner);
    Location_destroy(forest);
    Location_destroy(pool);
    Location_destroy(spaceship);
    Location_destroy(ork);
}

void populate_game(struct Database *db)
{
    generate_characters(db);
    generate_items(db);
    generate_locations(db);
}