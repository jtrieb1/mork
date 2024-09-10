#pragma once

#include <mork/coredb/db.h>

#include <stdio.h>
#include <stdlib.h>

void populate_game(struct Database *db);

void generate_characters(struct Database *db);
void generate_items(struct Database *db);
void generate_locations(struct Database *db);