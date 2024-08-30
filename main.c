/*
Mork: A Zorklike text adventure game influenced by classic late 70s television.
Copyright (C) 2024 Jacob Triebwasser

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <lcthw/dbg.h>
#include <sys/stat.h>

#include "src/coredb/db.h"

const char *title = ""
"_______  _______  _______  _             \n"
"(       )(  ___  )(  ____ )| \\    /\\   \n"
"| () () || (   ) || (    )||  \\  / /    \n"
"| || || || |   | || (____)||  (_/ /      \n"
"| |(_)| || |   | ||     __)|   _ (       \n"
"| |   | || |   | || (\\ (   |  ( \\ \\   \n"
"| )   ( || (___) || ) \\ \\__|  /  \\ \\ \n"
"|/     \\|(_______)|/   \\__/|_/    \\/  \n"
"                                         \n"
"\n";

const char *intro_text = ""
"MORK I: The Cool Aboveground Pool\n"
"Interactive Fiction - A Poorly-Written Parody\n"
"Written by Jacob Triebwasser, contact at \n"
"jacob.triebwasser@gmail.com\n";

const char *dir_name = "/tmp/morkdata";
const char *dbname = "mork.db";

struct Database *game_db = NULL;

void setup()
{
    char *full_path = malloc(strlen(dir_name) + strlen(dbname) + 2);
    check_mem(full_path);
    sprintf(full_path, "%s/%s", dir_name, dbname);

    // Check if game directory exists
    struct stat st = {0};
    if (stat(dir_name, &st) == 0) {
        debug("Directory already exists, skipping creation.");
    } else {
        debug("Creating directory %s", dir_name);
        int rc = mkdir(dir_name, 0700);
        check(rc == 0, "Could not create temporary directory");

        debug("Creating blank database file %s", dbname);

        FILE *db_file = fopen(full_path, "w");
        check(db_file != NULL, "Could not create database file");
        fclose(db_file);
    }

    // Spin up database
    game_db = Database_create();
    Database_open(game_db, full_path);
    check(game_db, "Could not open Mork database");

    printf("%s\n", title);
    printf("%s\n", intro_text);

    free(full_path);
    return;

error:
    free(full_path);
    exit(1);
}

int main()
{
    setup();
    check(game_db != NULL, "Database not initialized");

    // Note: Game database should be prepopulated with all assets for the game.
    // Tools for this are not yet implemented, but every table has working write methods.
    // Current goal is to turn everything in src into a Mork engine library for doing Zorklikes
    // and the database is the heart of that.

error:
    if (game_db != NULL) {
        Database_destroy(game_db);
    }
    return 0;
}
