#include "minunit.h"

#include "../src/models/character.h"
#include "../src/models/inventory.h"
#include "../src/models/item.h"

#include <stdio.h>

struct Character *mork = NULL;
struct Inventory *inventory = NULL;
struct Item *item = NULL;

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

    return NULL;
}

char *test_create_inventory()
{
    inventory = Inventory_create();
    mu_assert(inventory != NULL, "Failed to create inventory.");

    return NULL;
}

char *test_create_item()
{
    item = Item_create("Mork's Suspenders", "The rainbow suspenders commonly worn by Mork.");
    mu_assert(item != NULL, "Failed to create item.");
    mu_assert(strcmp(item->name, "Mork's Suspenders") == 0, "Failed to set name.");
    mu_assert(strcmp(item->description, "The rainbow suspenders commonly worn by Mork.") == 0, "Failed to set description.");

    return NULL;
}

char *test_add_item_to_inventory()
{
    Inventory_addItem(inventory, item);
    mu_assert(Inventory_getItemCount(inventory) == 1, "Failed to add item to inventory.");

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

char *test_destroy_inventory()
{
    Inventory_destroy(inventory); // Infallible
    return NULL;
}

char *test_destroy_character()
{
    Character_destroy(mork); // Infallible
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create_character);
    mu_run_test(test_create_inventory);
    mu_run_test(test_create_item);
    mu_run_test(test_add_item_to_inventory);
    mu_run_test(test_remove_item_from_inventory);
    mu_run_test(test_destroy_item);
    mu_run_test(test_destroy_inventory);
    mu_run_test(test_destroy_character);

    return NULL;
}

RUN_TESTS(all_tests);
