#include "minunit.h"
#include "test_settings.h"

#include "../src/coredb/db.h"
#include "../src/models/character.h"
#include "../src/models/inventory.h"
#include "../src/models/item.h"

#include <stdio.h>

struct Database *db = NULL;
struct Character *mork = NULL;
struct Inventory *inventory = NULL;
struct Item *item = NULL;

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
    mork = Character_create(
        "Mork",
        5, // strength
        5, // dexterity
        5, // intelligence
        5, // wisdom
        5, // charisma
        10 // funkiness
    );
    inventory = mork->inventory;

    mu_assert(mork != NULL, "Failed to create character.");
    mu_assert(strcmp(mork->name, "Mork") == 0, "Failed to set name.");
    mu_assert(mork->level == 1, "Failed to set level.");
    mu_assert(mork->experience == 0, "Failed to set experience.");
    mu_assert(mork->health == 100, "Failed to set health.");
    mu_assert(mork->max_health == 100, "Failed to set max_health.");
    mu_assert(mork->mana == 100, "Failed to set mana.");
    mu_assert(mork->max_mana == 100, "Failed to set max_mana.");
    mu_assert(mork->strength == 5, "Failed to set strength.");
    mu_assert(mork->dexterity == 5, "Failed to set dexterity.");
    mu_assert(mork->intelligence == 5, "Failed to set intelligence.");
    mu_assert(mork->wisdom == 5, "Failed to set wisdom.");
    mu_assert(mork->charisma == 5, "Failed to set charisma.");
    mu_assert(mork->funkiness == 10, "Failed to set funkiness.");
    mu_assert(mork->inventory != NULL, "Failed to create inventory.");

    Character_save(db, mork); // Save mork to the database

    // Now try loading Mork
    struct Character *mork2 = Character_load(db, "Mork");
    mu_assert(mork2 != NULL, "Failed to load character.");
    mu_assert(strcmp(mork2->name, "Mork") == 0, "Failed to load name.");
    mu_assert(mork2->level == 1, "Failed to load level.");
    mu_assert(mork2->experience == 0, "Failed to load experience.");
    mu_assert(mork2->health == 100, "Failed to load health.");
    mu_assert(mork2->max_health == 100, "Failed to load max_health.");
    mu_assert(mork2->mana == 100, "Failed to load mana.");
    mu_assert(mork2->max_mana == 100, "Failed to load max_mana.");
    mu_assert(mork2->strength == 5, "Failed to load strength.");
    mu_assert(mork2->dexterity == 5, "Failed to load dexterity.");
    mu_assert(mork2->intelligence == 5, "Failed to load intelligence.");
    mu_assert(mork2->wisdom == 5, "Failed to load wisdom.");
    mu_assert(mork2->charisma == 5, "Failed to load charisma.");
    mu_assert(mork2->funkiness == 10, "Failed to load funkiness.");
    mu_assert(mork2->inventory != NULL, "Failed to load inventory.");

    // Make sure we destroy the second Mork
    // There can, of course, only be one Mork.
    Character_destroy(mork2);

    return NULL;
}

char *test_create_item()
{
    item = Item_create("Mork's Suspenders", "The rainbow suspenders commonly worn by Mork.");
    mu_assert(item != NULL, "Failed to create item.");
    mu_assert(strcmp(item->name, "Mork's Suspenders") == 0, "Failed to set name.");
    mu_assert(strcmp(item->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to set description.");

    // Try saving the item
    int id = Item_save(db, item); // The ID returned is the ID of the item in the database
    mu_assert(id != -1, "Failed to save item.");

    // Now try loading the item
    struct Item *item2 = Item_load(db, id);
    mu_assert(item2 != NULL, "Failed to load item.");
    mu_assert(strcmp(item2->name, "Mork's Suspenders") == 0, "Failed to load name.");
    mu_assert(strcmp(item2->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to load description.");

    // Make sure we destroy the second item
    // There can, of course, only be one pair of Mork's suspenders.
    Item_destroy(item2);

    return NULL;
}

char *test_destroy_and_reload_character()
{
    Character_destroy(mork);
    mork = Character_load(db, "Mork");
    mu_assert(mork != NULL, "Failed to load character.");

    return NULL;
}

char *test_add_item_to_inventory()
{
    Inventory_addItem(inventory, item);
    mu_assert(Inventory_getItemCount(inventory) == 1, "Failed to add item to inventory.");

    // Write everything to the DB by writing the character
    Character_save(db, mork);

    // Now destroy and reload Mork
    char *result = test_destroy_and_reload_character();
    mu_assert(result == NULL, "Failed to destroy and reload character.");

    // Make sure the item is still in the inventory
    // and that the inventory is still there
    inventory = mork->inventory;
    mu_assert(inventory != NULL, "Failed to load inventory.");
    mu_assert(Inventory_getItemCount(inventory) == 1, "Failed to load item count.");

    return NULL;
}

char *test_remove_item_from_inventory()
{
    Inventory_removeItem(inventory, item);
    mu_assert(Inventory_getItemCount(inventory) == 0, "Failed to remove item from inventory.");

    return NULL;
}

char *test_destroy_item()
{
    Item_destroy(item); // Infallible
    return NULL;
}

char *test_destroy_character()
{
    Character_destroy(mork); // Infallible
    return NULL;
}

char *test_destroy_db()
{
    Database_close(db);
    Database_destroy(db); // Infallible
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
    mu_run_test(test_add_item_to_inventory);
    mu_run_test(test_remove_item_from_inventory);
    mu_run_test(test_destroy_item);
    mu_run_test(test_destroy_character);
    mu_run_test(test_destroy_db);
    mu_run_test(test_destroy_db_file);

    return NULL;
}

RUN_TESTS(all_tests);
