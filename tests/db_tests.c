#include "minunit.h"
#include "test_settings.h"

#include "../src/coredb/db.h"
#include "../src/utils/error.h"

#include <stdio.h>

struct Database *db = NULL;

char *test_create_db_file()
{
    enum MorkResult res = Database_createFile(db, test_db);
    mu_assert(res == MORK_OK, "Failed to create test database file.");

    return NULL;
}

char *test_create()
{
    db = Database_create();
    mu_assert(db != NULL, "Failed to create database.");

    return NULL;
}

char *test_destroy()
{
    mu_assert(db != NULL, "Expected DB to exist");

    Database_destroy(db);

    return NULL;
}

char *test_open()
{
    Database_open(db, test_db);
    mu_assert(db != NULL, "Failed to open database.");

    return NULL;
}

char *test_flush()
{
    Database_flush(db);

    // Check that the file has a size greater than 0
    fseek(db->file, 0, SEEK_END);
    long size = ftell(db->file);
    mu_assert(size > 0, "File size is 0.");

    return NULL;
}

char *test_close()
{
    Database_close(db);
    mu_assert(db->file == NULL, "Failed to close database.");

    return NULL;
}

char *test_delete_db_file()
{
    int rc = remove(test_db);
    mu_assert(rc == 0, "Failed to delete test database file.");

    return NULL;
}

char *test_create_character()
{
    struct CharacterRecord character = { .id = 1, .name = "Test Character" };
    enum MorkResult result = Database_createCharacter(db, &character);
    mu_assert(result == MORK_OK, "Failed to create character.");

    struct CharacterRecord *retrieved = Database_getCharacter(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve character.");
    mu_assert(strcmp(retrieved->name, "Test Character") == 0, "Character name mismatch.");

    return NULL;
}

char *test_update_character()
{
    struct CharacterRecord character = { .id = 1, .name = "Updated Character" };
    enum MorkResult result = Database_updateCharacter(db, &character);
    mu_assert(result == MORK_OK, "Failed to update character.");

    struct CharacterRecord *retrieved = Database_getCharacter(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve character.");
    mu_assert(strcmp(retrieved->name, "Updated Character") == 0, "Character name mismatch.");

    return NULL;
}

char *test_delete_character()
{
    enum MorkResult result = Database_deleteCharacter(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete character.");

    struct CharacterRecord *retrieved = Database_getCharacter(db, 1);
    mu_assert(retrieved == NULL, "Character was not deleted.");

    return NULL;
}

char *test_create_item()
{
    struct ItemRecord item = { .id = 1, .name = "Test Item" };
    enum MorkResult result = Database_createItem(db, &item);
    mu_assert(result == MORK_OK, "Failed to create item.");

    struct ItemRecord *retrieved = Database_getItem(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve item.");
    mu_assert(strcmp(retrieved->name, "Test Item") == 0, "Item name mismatch.");

    return NULL;
}

char *test_update_item()
{
    struct ItemRecord item = { .id = 1, .name = "Updated Item" };
    enum MorkResult result = Database_updateItem(db, &item);
    mu_assert(result == MORK_OK, "Failed to update item.");

    struct ItemRecord *retrieved = Database_getItem(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve item.");
    mu_assert(strcmp(retrieved->name, "Updated Item") == 0, "Item name mismatch.");

    return NULL;
}

char *test_delete_item()
{
    enum MorkResult result = Database_deleteItem(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete item.");

    struct ItemRecord *retrieved = Database_getItem(db, 1);
    mu_assert(retrieved == NULL, "Item was not deleted.");

    return NULL;
}

char *test_create_location()
{
    struct LocationRecord location = { .id = 1, .name = "Test Location" };
    enum MorkResult result = Database_createLocation(db, &location);
    mu_assert(result == MORK_OK, "Failed to create location.");

    struct LocationRecord *retrieved = Database_getLocation(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve location.");
    mu_assert(strcmp(retrieved->name, "Test Location") == 0, "Location name mismatch.");

    return NULL;
}

char *test_update_location()
{
    struct LocationRecord location = { .id = 1, .name = "Updated Location" };
    enum MorkResult result = Database_updateLocation(db, &location);
    mu_assert(result == MORK_OK, "Failed to update location.");

    struct LocationRecord *retrieved = Database_getLocation(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve location.");
    mu_assert(strcmp(retrieved->name, "Updated Location") == 0, "Location name mismatch.");

    return NULL;
}

char *test_delete_location()
{
    enum MorkResult result = Database_deleteLocation(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete location.");

    struct LocationRecord *retrieved = Database_getLocation(db, 1);
    mu_assert(retrieved == NULL, "Location was not deleted.");

    return NULL;
}

char *test_create_description()
{
    struct DescriptionRecord description = { .id = 1, .description = "Test Description" };
    enum MorkResult result = Database_createDescription(db, &description);
    mu_assert(result == MORK_OK, "Failed to create description.");

    struct DescriptionRecord *retrieved = Database_getDescription(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve description.");
    mu_assert(strcmp(retrieved->description, "Test Description") == 0, "Description prefix mismatch.");

    return NULL;
}

char *test_update_description()
{
    struct DescriptionRecord description = { .id = 1, .description = "Updated Description" };
    enum MorkResult result = Database_updateDescription(db, &description);
    mu_assert(result == MORK_OK, "Failed to update description.");

    struct DescriptionRecord *retrieved = Database_getDescription(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve description.");
    mu_assert(strcmp(retrieved->description, "Updated Description") == 0, "Description prefix mismatch.");

    return NULL;
}

char *test_delete_description()
{
    enum MorkResult result = Database_deleteDescription(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete description.");

    struct DescriptionRecord *retrieved = Database_getDescription(db, 1);
    mu_assert(retrieved == NULL, "Description was not deleted.");

    return NULL;
}

char *test_create_dialog()
{
    struct DialogRecord dialog = { .id = 1, .text = "Test Dialog" };
    enum MorkResult result = Database_createDialog(db, &dialog);
    mu_assert(result == MORK_OK, "Failed to create dialog.");

    struct DialogRecord *retrieved = Database_getDialog(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve dialog.");
    mu_assert(strcmp(retrieved->text, "Test Dialog") == 0, "Dialog text mismatch.");

    return NULL;
}

char *test_update_dialog()
{
    struct DialogRecord dialog = { .id = 1, .text = "Updated Dialog" };
    enum MorkResult result = Database_updateDialog(db, &dialog);
    mu_assert(result == MORK_OK, "Failed to update dialog.");

    struct DialogRecord *retrieved = Database_getDialog(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve dialog.");
    mu_assert(strcmp(retrieved->text, "Updated Dialog") == 0, "Dialog text mismatch.");

    return NULL;
}

char *test_delete_dialog()
{
    enum MorkResult result = Database_deleteDialog(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete dialog.");

    struct DialogRecord *retrieved = Database_getDialog(db, 1);
    mu_assert(retrieved == NULL, "Dialog was not deleted.");

    return NULL;
}

char *test_create_inventory()
{
    // Create owner first
    struct CharacterRecord owner = { .id = 1, .name = "Test Character" };
    enum MorkResult result = Database_createCharacter(db, &owner);
    mu_assert(result == MORK_OK, "Failed to create character.");

    result = Database_createInventory(db, owner.name);
    mu_assert(result == MORK_OK, "Failed to create inventory.");

    struct InventoryRecord *retrieved = Database_getInventory(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve inventory.");
    mu_assert(retrieved->owner_id == 1, "Inventory owner mismatch.");

    return NULL;
}

char *test_update_inventory()
{
    struct InventoryRecord inventory = { .id = 1, .owner_id = 1 };
    enum MorkResult result = Database_updateInventory(db, &inventory);
    mu_assert(result == MORK_OK, "Failed to update inventory.");

    struct InventoryRecord *retrieved = Database_getInventory(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve inventory.");
    mu_assert(retrieved->owner_id == 1, "Inventory owner mismatch.");

    return NULL;
}

char *test_delete_inventory()
{
    enum MorkResult result = Database_deleteInventory(db, 1);
    mu_assert(result == MORK_OK, "Failed to delete inventory.");

    struct InventoryRecord *retrieved = Database_getInventory(db, 1);
    mu_assert(retrieved == NULL, "Inventory was not deleted.");

    return NULL;
}

char *test_destroy_and_reopen()
{
    db = Database_create();
    Database_createFile(db, test_db);
    Database_open(db, test_db);
    // Create a character
    struct CharacterRecord character = { .id = 1, .name = "Test Character" };
    enum MorkResult result = Database_createCharacter(db, &character);
    mu_assert(result == MORK_OK, "Failed to create character.");

    // Flush and close the database
    Database_flush(db);
    Database_close(db);
    Database_destroy(db);

    // Reopen the database
    db = Database_create();
    Database_open(db, test_db);
    mu_assert(db != NULL, "Failed to reopen database.");

    // Retrieve the character and check its data
    struct CharacterRecord *retrieved = Database_getCharacter(db, 1);
    mu_assert(retrieved != NULL, "Failed to retrieve character after reopening.");
    mu_assert(strcmp(retrieved->name, "Test Character") == 0, "Character name mismatch after reopening.");

    // Clean up
    Database_deleteCharacter(db, 1);
    Database_close(db);
    Database_destroy(db);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_create_db_file);
    mu_run_test(test_open);
    mu_run_test(test_create_character);
    mu_run_test(test_update_character);
    mu_run_test(test_delete_character);
    mu_run_test(test_create_item);
    mu_run_test(test_update_item);
    mu_run_test(test_delete_item);
    mu_run_test(test_create_location);
    mu_run_test(test_update_location);
    mu_run_test(test_delete_location);
    mu_run_test(test_create_description);
    mu_run_test(test_update_description);
    mu_run_test(test_delete_description);
    mu_run_test(test_create_dialog);
    mu_run_test(test_update_dialog);
    mu_run_test(test_delete_dialog);
    mu_run_test(test_create_inventory);
    mu_run_test(test_update_inventory);
    mu_run_test(test_delete_inventory);
    mu_run_test(test_flush);
    mu_run_test(test_close);
    mu_run_test(test_destroy);
    mu_run_test(test_delete_db_file);
    mu_run_test(test_destroy_and_reopen);

    return NULL;
}

RUN_TESTS(all_tests);