#include "minunit.h"
#include "test_settings.h"

#include "../src/coredb/db.h"
#include "../src/models/character.h"
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
    Item_destroy(item2);
    Item_destroy(item);

    return NULL;
}

char *test_create_multiple_items()
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

    // Now create a new item
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
    Location_destroy(location);           // Destroy the location. We'll load it from the database in a moment.

    // Now try loading the location
    location = Location_loadByName(db, "Mork's House");
    mu_assert(location != NULL, "Failed to load location.");
    mu_assert(strcmp(location->name, "Mork's House") == 0, "Failed to load name.");
    mu_assert(strcmp(location->description, "The home of Mork.") == 0, "Failed to load description.");

    Location_destroy(location);

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

char *test_destroy_db()
{
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
    mu_run_test(test_destroy_db);
    mu_run_test(test_destroy_db_file);

    return NULL;
}

RUN_TESTS(all_tests);
