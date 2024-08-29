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

#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct MorkConfig {
    const char* character_db_path;
    const char* location_db_path;
};

struct MorkConfig *MorkConfig_default();
void MorkConfig_free(struct MorkConfig *config);
void MorkConfig_print(struct MorkConfig *config);
void MorkConfig_set_base_path(struct MorkConfig *config, const char *path);
struct MorkConfig *MorkConfig_load(const char *path);
