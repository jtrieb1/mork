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

#define  _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <lcthw/dbg.h>
#include <sys/stat.h>

#include "src/config.h"

struct MorkConfig *config = NULL;

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

struct Database *game_db = NULL;

void setup()
{
    // Load config file
    config = MorkConfig_default();
    if (config == NULL) {
        log_err("Could not load config file.");
    }

    MorkConfig_set_base_path(config, dir_name);

    // Check if game directory exists
    struct stat st = {0};
    if (stat(dir_name, &st) == 0) {
        debug("Directory already exists, skipping creation.");
    } else {
        debug("Creating directory %s", dir_name);
        int rc = mkdir(dir_name, 0700);
        check(rc == 0, "Could not create temporary directory");

        debug("Creating blank database file %s", config->character_db_path);
        FILE *db_file = fopen(config->character_db_path, "w");
        check(db_file != NULL, "Could not create database file");
        fclose(db_file);
    }

    // Spin up database
    game_db = Database_create();
    Database_open(game_db, config->character_db_path);
    check(game_db, "Could not open Mork database");

    printf("%s\n", title);
    printf("%s\n", intro_text);
error:
    exit(1);
}

int main()
{
    setup();
    MorkConfig_free(config);
    return 0;
}
