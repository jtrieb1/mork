#include "minunit.h"
#include "test_settings.h"

#include "../src/coredb/db.h"
#include "../src/models/action.h"
#include "../src/models/character.h"
#include "../src/models/game.h"
#include "../src/models/inventory.h"
#include "../src/models/item.h"
#include "../src/models/location.h"

#include <stdio.h>

struct Database *db = NULL;

enum Stats {
    STRENGTH,
    DEXTERITY,
    INTELLIGENCE,
    CHARISMA,
    WISDOM,
    FUNKINESS
};

char *test_create_db_file()
{
    FILE *file = fopen(test_db, "w");
    mu_assert(file != NULL, "Failed to create test database file.");

    fclose(file);

    return NULL;
}

char *test_create_db()
{
    db = Database_create();
    mu_assert(db != NULL, "Failed to create database.");

    // Also open it
    Database_open(db, test_db);

    return NULL;
}

char *test_create_character()
{
    struct Character *mork = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    mu_assert(mork != NULL, "Failed to create character.");
    mu_assert(strcmp(mork->name, "Mork") == 0, "Failed to set name.");
    mu_assert(mork->level == 1, "Failed to set level.");
    mu_assert(mork->experience == 0, "Failed to set experience.");
    mu_assert(mork->health == 110, "Failed to set health.");
    mu_assert(mork->max_health == 110, "Failed to set max_health.");
    mu_assert(mork->mana == 60, "Failed to set mana.");
    mu_assert(mork->max_mana == 60, "Failed to set max_mana.");
    mu_assert(Character_getStat(mork, STRENGTH) == 5, "Failed to set strength.");
    mu_assert(Character_getStat(mork, DEXTERITY) == 5, "Failed to set dexterity.");
    mu_assert(Character_getStat(mork, INTELLIGENCE) == 5, "Failed to set intelligence.");
    mu_assert(Character_getStat(mork, WISDOM) == 5, "Failed to set wisdom.");
    mu_assert(Character_getStat(mork, CHARISMA) == 5, "Failed to set charisma.");
    mu_assert(Character_getStat(mork, FUNKINESS) == 10, "Failed to set funkiness.");
    mu_assert(mork->inventory != NULL, "Failed to create inventory.");

    Character_save(db, mork); // Save mork to the database
    Character_destroy(mork); // Destroy Mork. Don't worry, we saved him.
    mork = NULL;

    // Now try loading Mork
    mork = Character_load(db, "Mork");
    mu_assert(mork != NULL, "Failed to load character.");
    mu_assert(strcmp(mork->name, "Mork") == 0, "Failed to load name.");
    mu_assert(mork->level == 1, "Failed to load level.");
    mu_assert(mork->experience == 0, "Failed to load experience.");
    mu_assert(mork->health == 110, "Failed to load health.");
    mu_assert(mork->max_health == 110, "Failed to load max_health.");
    mu_assert(mork->mana == 60, "Failed to load mana.");
    mu_assert(mork->max_mana == 60, "Failed to load max_mana.");
    mu_assert(Character_getStat(mork, STRENGTH) == 5, "Failed to load strength.");
    mu_assert(Character_getStat(mork, DEXTERITY) == 5, "Failed to load dexterity.");
    mu_assert(Character_getStat(mork, INTELLIGENCE) == 5, "Failed to load intelligence.");
    mu_assert(Character_getStat(mork, WISDOM) == 5, "Failed to load wisdom.");
    mu_assert(Character_getStat(mork, CHARISMA) == 5, "Failed to load charisma.");
    mu_assert(Character_getStat(mork, FUNKINESS) == 10, "Failed to load funkiness.");
    mu_assert(mork->inventory != NULL, "Failed to load inventory.");

    Character_destroy(mork); // Mork will be back.

    return NULL;
}

char *test_create_item()
{
    struct Item *item = Item_create("Mork's Suspenders", "The rainbow suspenders commonly worn by Mork.");
    mu_assert(item != NULL, "Failed to create item.");
    mu_assert(strcmp(item->name, "Mork's Suspenders") == 0, "Failed to set name.");
    mu_assert(strcmp(item->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to set description.");

    // Try saving the item
    int id = Item_save(db, item); // The ID returned is the ID of the item in the database
    Item_destroy(item);           // Destroy the item. We'll load it from the database in a moment.
    item = NULL;
    mu_assert(id != -1, "Failed to save item.");

    // Now try loading the item
    item = Item_load(db, id);
    mu_assert(item != NULL, "Failed to load item.");
    mu_assert(strcmp(item->name, "Mork's Suspenders") == 0, "Failed to load name.");
    mu_assert(strcmp(item->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to load description.");

    // Now try cloning the item
    struct Item *item2 = Item_clone(item);
    mu_assert(item2 != NULL, "Failed to load item.");
    mu_assert(strcmp(item2->name, "Mork's Suspenders") == 0, "Failed to load name.");
    mu_assert(strcmp(item2->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to load description.");

    // Make sure we destroy the clone
    // There can, of course, only be one pair of Mork's suspenders.
    enum MorkResult res = Item_destroy(item2);
    mu_assert(res == MORK_OK, "Failed to destroy item.");
    res = Item_destroy(item);
    mu_assert(res == MORK_OK, "Failed to destroy item.");

    return NULL;
}

char *test_create_multiple_items()
{
    // Load Mork's suspenders
    struct Item *item = Item_loadByName(db, "Mork's Suspenders");
    mu_assert(item != NULL, "Failed to load item.");

    // Create a new item
    struct Item *item2 = Item_create("Mork's Hat", "The red hat commonly worn by Mork.");
    mu_assert(item2 != NULL, "Failed to create item.");
    mu_assert(strcmp(item2->name, "Mork's Hat") == 0, "Failed to set name.");
    mu_assert(strcmp(item2->description, "The red hat commonly worn by Mork.") == 0, "Failed to set description.");

    // Try saving the item
    int id2 = Item_save(db, item2); // The ID returned is the ID of the item in the database
    Item_destroy(item2);           // Destroy the item. We'll load it from the database in a moment.
    item2 = NULL;
    mu_assert(id2 != -1, "Failed to save item.");

    // Now try loading the item
    item2 = Item_load(db, id2);
    mu_assert(item2 != NULL, "Failed to load item.");
    mu_assert(strcmp(item2->name, "Mork's Hat") == 0, "Failed to load name.");
    mu_assert(strcmp(item2->description, "The red hat commonly worn by Mork.") == 0, "Failed to load description.");

    // Make sure the items have different IDs
    mu_assert(item->id != item2->id, "Failed to create unique IDs for items.");

    // Make sure we destroy the items
    Item_destroy(item);
    Item_destroy(item2);

    return NULL;
}

char *test_add_item_to_inventory()
{
    struct Character *mork = Character_load(db, "Mork");
    mu_assert(mork != NULL, "Failed to load Mork");
    Database_print(db, ITEMS);
    struct Item *item = Item_loadByName(db, "Mork's Suspenders"); // Load Mork's suspenders
    mu_assert(item != NULL, "Failed to load item.");

    mu_assert(Inventory_addItem(mork->inventory, item) == MORK_OK, "Failed to add item to inventory.");
    mu_assert(Inventory_getItemCount(mork->inventory) == 1, "Failed to load item count.");
    log_info("Item count: %d", Inventory_getItemCount(mork->inventory));

    // Write everything to the DB by writing the character
    Character_save(db, mork);

    // Now destroy and reload Mork
    Character_destroy(mork);
    mork = Character_load(db, "Mork");

    // Make sure the item is still in the inventory
    // and that the inventory is still there
    mu_assert(mork->inventory != NULL, "Failed to load inventory.");
    log_info("Item count on load: %d", Inventory_getItemCount(mork->inventory));
    mu_assert(Inventory_getItemCount(mork->inventory) == 1, "Failed to load item count.");

    Character_destroy(mork);

    return NULL;
}

char *test_remove_item_from_inventory()
{
    struct Character *mork = Character_load(db, "Mork");
    mu_assert(mork != NULL, "Failed to load Mork");
    struct Item *item = Item_loadByName(db, "Mork's Suspenders");
    log_info("Item ID: %d", item->id);
    mu_assert(item != NULL, "Failed to load item.");

    mu_assert(Inventory_removeItem(mork->inventory, item) == MORK_OK, "Failed to find item to remove.");
    mu_assert(Inventory_getItemCount(mork->inventory) == 0, "Failed to remove item from inventory.");

    // Write everything to the DB by writing the character
    Character_save(db, mork);

    // Destroy and reload Mork and make sure the inventory is still empty
    Character_destroy(mork);
    mork = Character_load(db, "Mork");

    mu_assert(mork->inventory != NULL, "Failed to load inventory.");
    log_err("Item count: %d", Inventory_getItemCount(mork->inventory));
    mu_assert(Inventory_getItemCount(mork->inventory) == 0, "Failed to save updated item count");

    Character_destroy(mork);
    Item_destroy(item); // Item doesn't belong to Mork, so he doesn't delete it.

    return NULL;
}

char *test_create_location()
{
    struct Location *location = Location_create("Mork's House", "The home of Mork.");
    mu_assert(location != NULL, "Failed to create location.");
    mu_assert(strcmp(location->name, "Mork's House") == 0, "Failed to set name.");
    mu_assert(strcmp(location->description, "The home of Mork.") == 0, "Failed to set description.");

    // Try saving the location
    enum MorkResult res = Location_save(db, location); 
    mu_assert(res == MORK_OK, "Failed to save location.");
    res = Location_destroy(location);           // Destroy the location. We'll load it from the database in a moment.
    mu_assert(res == MORK_OK, "Failed to destroy location.");

    // Now try loading the location
    location = Location_loadByName(db, "Mork's House");
    mu_assert(location != NULL, "Failed to load location.");
    mu_assert(strcmp(location->name, "Mork's House") == 0, "Failed to load name.");
    mu_assert(strcmp(location->description, "The home of Mork.") == 0, "Failed to load description.");

    res = Location_destroy(location);
    mu_assert(res == MORK_OK, "Failed to destroy location.");

    return NULL;
}

char *test_create_multiple_different_locations()
{
    struct Location *location = Location_create("Mork's Other House", "The other home of Mork.");
    mu_assert(location != NULL, "Failed to create location.");
    mu_assert(strcmp(location->name, "Mork's Other House") == 0, "Failed to set name.");
    mu_assert(strcmp(location->description, "The other home of Mork.") == 0, "Failed to set description.");

    // Try saving the location
    enum MorkResult res = Location_save(db, location); 
    mu_assert(res == MORK_OK, "Failed to save location.");
    Location_destroy(location);          

    // Create a new location
    struct Location *mork_garden = Location_create("Mork's Garden", "The garden of Mork.");
    mu_assert(mork_garden != NULL, "Failed to create Mork's Garden.");
    mu_assert(strcmp(mork_garden->name, "Mork's Garden") == 0, "Failed to set name.");
    mu_assert(strcmp(mork_garden->description, "The garden of Mork.") == 0, "Failed to set description.");

    // Try saving the location
    res = Location_save(db, mork_garden);
    mu_assert(res == MORK_OK, "Failed to save Mork's Garden.");
    Location_destroy(mork_garden);           

    // Now try loading the locations
    mork_garden = Location_loadByName(db, "Mork's Garden");
    mu_assert(mork_garden != NULL, "Failed to load Mork's Garden.");
    mu_assert(strcmp(mork_garden->name, "Mork's Garden") == 0, "Failed to load name.");
    mu_assert(strcmp(mork_garden->description, "The garden of Mork.") == 0, "Failed to load description.");

    struct Location *mork_house = Location_loadByName(db, "Mork's Other House");
    mu_assert(mork_house != NULL, "Failed to load Mork's Other House.");
    mu_assert(strcmp(mork_house->name, "Mork's Other House") == 0, "Failed to load name.");
    mu_assert(strcmp(mork_house->description, "The other home of Mork.") == 0, "Failed to load description.");

    // Make sure the locations have different IDs
    mu_assert(mork_garden->id != mork_house->id, "Failed to create unique IDs for locations.");

    Location_destroy(mork_house);
    Location_destroy(mork_garden);

    return NULL;
}

char *test_add_item_to_location()
{
    struct Location *location = Location_loadByName(db, "Mork's House");
    mu_assert(location != NULL, "Failed to create location.");
    Location_save(db, location);

    struct Item *item = Item_loadByName(db, "Mork's Suspenders");
    mu_assert(item != NULL, "Failed to load item.");

    mu_assert(Location_addItem(location, item) == MORK_OK, "Failed to add item to location.");
    mu_assert(Location_getItemCount(location) == 1, "Failed to load item count.");

    enum MorkResult res = Location_save(db, location);
    mu_assert(res == MORK_OK, "Failed to save location.");

    res = Location_destroy(location);
    mu_assert(res == MORK_OK, "Failed to destroy location.");
    res = Item_destroy(item);
    mu_assert(res == MORK_OK, "Failed to destroy item.");

    return NULL;
}

char *test_remove_item_from_location()
{
    struct Location *location = Location_loadByName(db, "Mork's House");
    mu_assert(location != NULL, "Failed to load location.");
    mu_assert(Location_getItemCount(location) == 1, "Failed to load location item count.");

    struct Item *item = Item_loadByName(db, "Mork's Suspenders");
    mu_assert(item != NULL, "Failed to load item.");

    mu_assert(Location_removeItem(location, item) == MORK_OK, "Failed to remove item from location.");
    mu_assert(Location_getItemCount(location) == 0, "Failed to update location item count.");

    Location_save(db, location);

    Location_destroy(location);
    Item_destroy(item);

    return NULL;
}

char *test_update_item_in_location()
{
    struct Location *location = Location_loadByName(db, "Mork's House");
    mu_assert(location != NULL, "Failed to load location.");

    struct Item *item = Item_loadByName(db, "Mork's Suspenders");
    mu_assert(item != NULL, "Failed to load item.");

    strcpy(item->description, "Updated description for Mork's Suspenders.");
    mu_assert(Item_save(db, item) != -1, "Failed to update item.");

    struct Item *updated_item = Item_loadByName(db, "Mork's Suspenders");
    mu_assert(updated_item != NULL, "Failed to load updated item.");
    log_info("Updated item description: %s", updated_item->description);
    mu_assert(strcmp(updated_item->description, "Updated description for Mork's Suspenders.") == 0, "Failed to update item description.");

    Location_destroy(location);
    Item_destroy(item);
    Item_destroy(updated_item);

    return NULL;
}

char *test_destroy_db()
{
    // Cleanup
    Database_close(db);
    Database_destroy(db);
    return NULL;
}

char *test_destroy_db_file()
{
    int rc = remove(test_db);
    mu_assert(rc == 0, "Failed to remove test database file.");

    return NULL;
}

char *test_create_basegame()
{
    struct Character *player = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct BaseGame *game = BaseGame_create(player);
    mu_assert(game != NULL, "Failed to create game.");
    mu_assert(game->player != NULL, "Failed to set player.");
    mu_assert(game->current_location == NULL, "Failed to set current location.");

    BaseGame_destroy(game);

    return NULL;
}

char *test_load_basegame()
{
    struct Character *player = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct BaseGame *game = BaseGame_create(player);
    mu_assert(game != NULL, "Failed to create game.");
    mu_assert(game->player != NULL, "Failed to set player.");
    mu_assert(game->current_location == NULL, "Failed to set current location.");
    BaseGame_setLocation(game, Location_loadByName(db, "Mork's House"));
    BaseGame_save(db, game);

    BaseGame_destroy(game);

    // Now try to load it
    game = BaseGame_load(db, 1);
    mu_assert(game != NULL, "Failed to load game.");
    mu_assert(game->player != NULL, "Failed to load player.");
    mu_assert(game->current_location != NULL, "Failed to load current location.");

    BaseGame_destroy(game);

    return NULL;
}

char *test_create_action()
{
    const char *input = "look north";
    struct Action *action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    enum MorkResult parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "look") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_LOOK, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "north") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_NORTH, "Failed to parse target kind.");

    Action_destroy(action);

    input = "move down";
    action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "move") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_MOVE, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "down") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_DOWN, "Failed to parse target kind.");

    Action_destroy(action);

    return NULL;
}

char *test_parse_actions()
{
    const char *input = "look north";
    struct Action *action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    enum MorkResult parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "look") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_LOOK, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "north") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_NORTH, "Failed to parse target kind.");

    Action_destroy(action);

    input = "move south";
    action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "move") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_MOVE, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "south") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_SOUTH, "Failed to parse target kind.");

    Action_destroy(action);

    input = "inventory";
    action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "inventory") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_INVENTORY, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_NONE, "Failed to parse target kind.");

    Action_destroy(action);

    input = "help";
    action = Action_create(input);

    mu_assert(action != NULL, "Failed to create action.");
    mu_assert(strcmp(action->raw_input, input) == 0, "Failed to set raw input.");

    parseResult = Action_parse(action);
    mu_assert(parseResult == MORK_OK, "Failed to parse action.");

    mu_assert(strcmp(action->verb, "help") == 0, "Failed to parse verb.");
    mu_assert(action->kind == ACTION_HELP, "Failed to parse action kind.");
    mu_assert(strcmp(action->noun, "") == 0, "Failed to parse noun.");
    mu_assert(action->target_kind == TARGET_NONE, "Failed to parse target kind.");

    Action_destroy(action);

    return NULL;
}

char *test_execute_action()
{
    struct Character *player = Character_create(
        "Mork",
        1,                                            // Level
        (unsigned char[6]){5, 5, 5, 5, 5, 10},        // Stats in enum order
        6                                             // Number of stats
    );

    struct BaseGame *game = BaseGame_create(player);
    mu_assert(game != NULL, "Failed to create game.");
    mu_assert(game->player != NULL, "Failed to set player.");
    mu_assert(game->current_location == NULL, "Failed to set current location.");
    BaseGame_setLocation(game, Location_loadByName(db, "Mork's House"));

    const char *input = "look north";
    struct Action *action = Action_create(input);
    Action_parse(action);

    enum MorkResult res = BaseGame_executeAction(db, game, action);
    mu_assert(res == MORK_OK, "Failed to execute action.");

    struct Action *lastAction = game->history[0];
    mu_assert(lastAction != NULL, "Failed to save action to history.");
    mu_assert(strcmp(lastAction->verb, "look") == 0, "Failed to save verb to history.");
    mu_assert(lastAction->kind == ACTION_LOOK, "Failed to save action kind to history.");
    mu_assert(strcmp(lastAction->noun, "north") == 0, "Failed to save noun to history.");
    mu_assert(lastAction->target_kind == TARGET_NORTH, "Failed to save target kind to history.");

    input = "move down";
    action = Action_create(input);
    Action_parse(action);

    res = BaseGame_executeAction(db, game, action);
    mu_assert(res == MORK_OK, "Failed to execute action.");

    lastAction = game->history[0];
    mu_assert(lastAction != NULL, "Failed to save action to history.");
    mu_assert(strcmp(lastAction->verb, "move") == 0, "Failed to save verb to history.");
    mu_assert(lastAction->kind == ACTION_MOVE, "Failed to save action kind to history.");
    mu_assert(strcmp(lastAction->noun, "down") == 0, "Failed to save noun to history.");
    mu_assert(lastAction->target_kind == TARGET_DOWN, "Failed to save target kind to history.");

    struct Action *theOneBeforeThat = game->history[1];
    mu_assert(theOneBeforeThat != NULL, "Failed to save action to history.");
    log_info("The one before that: %s", theOneBeforeThat->raw_input);
    mu_assert(strcmp(theOneBeforeThat->verb, "look") == 0, "Failed to save verb to history.");
    mu_assert(theOneBeforeThat->kind == ACTION_LOOK, "Failed to save action kind to history.");
    mu_assert(strcmp(theOneBeforeThat->noun, "north") == 0, "Failed to save noun to history.");
    mu_assert(theOneBeforeThat->target_kind == TARGET_NORTH, "Failed to save target kind to history.");

    BaseGame_destroy(game);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create_db_file);
    mu_run_test(test_create_db);
    mu_run_test(test_create_character);
    mu_run_test(test_create_item);
    mu_run_test(test_create_multiple_items);
    mu_run_test(test_add_item_to_inventory);
    mu_run_test(test_remove_item_from_inventory);
    mu_run_test(test_create_location);
    mu_run_test(test_create_multiple_different_locations);
    mu_run_test(test_add_item_to_location);
    mu_run_test(test_remove_item_from_location);
    mu_run_test(test_update_item_in_location);
    mu_run_test(test_create_basegame);
    mu_run_test(test_load_basegame);
    mu_run_test(test_create_action);
    mu_run_test(test_parse_actions);
    mu_run_test(test_execute_action);
    mu_run_test(test_destroy_db);
    mu_run_test(test_destroy_db_file);

    return NULL;
}

RUN_TESTS(all_tests);
