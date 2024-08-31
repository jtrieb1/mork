#include "minunit.h"
#include "test_settings.h"

#include "../src/coredb/db.h"

#include <stdio.h>

struct Database *db = NULL;

char *test_create_db_file()
{
    FILE *file = fopen(test_db, "w");
    mu_assert(file != NULL, "Failed to create test database file.");

    fclose(file);

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

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create_db_file);
    mu_run_test(test_create);
    mu_run_test(test_open);
    mu_run_test(test_flush);
    mu_run_test(test_close);
    mu_run_test(test_destroy);
    mu_run_test(test_delete_db_file);

    return NULL;
}

RUN_TESTS(all_tests);
