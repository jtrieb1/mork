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

#include <mork/coredb/db.h>
#include <mork/models/character.h>
#include <mork/models/location.h>
#include <mork/models/game.h>
#include <mork/ui/terminal.h>
#include <sys/types.h>

#include "gamedata.h"
#include "text.h"

enum Stats {
    STRENGTH,
    DEXTERITY,
    INTELLIGENCE,
    CHARISMA,
    WISDOM,
    FUNKINESS
};

const char *dir_name = "/tmp/morkdata";
const char *dbname = "mork.db";

void set_title_text_format()
{
    set_text_bold();
    set_text_green();
}

void set_normal_text_format()
{
    set_text_normal();
    set_text_white();
}

void setup_game_directory(char *full_path)
{
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

    return;

error:
    exit(1);
}

struct Database *create_db(char *full_path)
{
    struct Database *game_db = Database_create();
    Database_createFile(game_db, full_path);

    return game_db;
}

void setup()
{
    char *full_path = malloc(strlen(dir_name) + strlen(dbname) + 2);
    check_mem(full_path);
    sprintf(full_path, "%s/%s", dir_name, dbname);

    setup_game_directory(full_path);

    struct Database *game_db = create_db(full_path);
    Database_open(game_db, full_path);
    
    populate_game(game_db);

    clear_screen();

    set_title_text_format();
    print_centered(TITLE);
    printf("\n");

    set_normal_text_format();
    printf("%s\n", INTRO);

    Database_destroy(game_db);
    free(full_path);
    return;

error:
    free(full_path);
    exit(1);
}

void refresh()
{
    clear_screen();
    set_cursor_to_screen_top();
    set_title_text_format();
    print_centered(TITLE);
    printf("\n\n");
    set_normal_text_format();
}

struct Character *create_player_menu(struct Database *db)
{
    char *name = NULL;
    unsigned int level = 1;
    unsigned char stats[6] = {5, 5, 5, 5, 5, 10};
    unsigned int numStats = 6;

    refresh();
    printf("Enter your character's name: ");
    int res = scanf("%ms", &name);
    check(res != EOF, "Could not read character name");

    struct Character *player = Character_create(name, level, stats, numStats);
    check(player != NULL, "Could not create player character");

    Character_save(db, player);

    return player;

error:
    return NULL;
}

int main()
{
    setup();

    struct Database *game_db = Database_create();
    check(game_db != NULL, "Could not create database");
    enum MorkResult result = Database_open(game_db, "/tmp/morkdata/mork.db");
    check(game_db != NULL, "Could not open database");
    log_info("Result of opening DB: %d", result);
    check(result == MORK_OK, "Could not open database");

    struct Character *player = create_player_menu(game_db);

    struct BaseGame *game = BaseGame_create(player);
    check(game != NULL, "Could not create game");

    struct Location *start = Location_loadByName(game_db, "Your Apartment");
    check(start != NULL, "Could not load starting location");

    BaseGame_setLocation(game, start);

    BaseGame_run(game_db, game);

    return 0;

error:
    return 1;
}
