#include "terminal.h"

#include <lcthw/dbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

void clear_screen()
{
    printf("\033[H\033[J\033[H");
    return;
}

void set_text_bold()
{
    printf("\033[1m");
    return;
}

void set_text_dim()
{
    printf("\033[2m");
    return;
}

void set_text_underlined()
{
    printf("\033[4m");
    return;
}

void set_text_blink()
{
    printf("\033[5m");
    return;
}

void set_text_normal()
{
    printf("\033[0m");
    return;
}

void set_text_green()
{
    printf("\033[32m");
    return;
}

void set_text_white()
{
    printf("\033[37m");
    return;
}

void set_text_red()
{
    printf("\033[31m");
    return;
}

void set_text_yellow()
{
    printf("\033[33m");
    return;
}

void set_text_blue()
{
    printf("\033[34m");
    return;
}

void set_cursor_to_screen_top()
{
    printf("\033[1;1H");
    return;
}

void set_cursor_to_screen_bottom()
{
    printf("\033[999;1H");
    return;
}

void set_cursor_to_screen_center()
{
    printf("\033[12;40H");
    return;
}

void set_cursor_to_line_start()
{
    printf("\033[1G");
    return;
}

void get_cursor_position(int *col, int *row)
{
    struct termios term, restore;

    tcgetattr(0, &term);
    tcgetattr(0, &restore);
    term.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(0, TCSANOW, &term);

    printf("\033[6n");
    scanf("\033[%d;%dR", row, col);

    tcsetattr(0, TCSANOW, &restore);
    return;
}

void get_cursor_row(int *row)
{
    int col;
    get_cursor_position(&col, row);
    return;
}

void get_cursor_col(int *col)
{
    int row;
    get_cursor_position(col, &row);
    return;
}

void set_cursor_to_col_row(int col, int row)
{
    printf("\033[%d;%dH", row, col);
    return;
}

void set_cursor_to_row(int line)
{
    printf("\033[%d;1H", line);
    return;
}

void set_cursor_to_line_position(int index)
{
    printf("\033[%dG", index);
    return;
}

void clear_line()
{
    printf("\033[2K");
    return;
}

void refresh_screen() {
    clear_screen();
    set_cursor_to_screen_top();
}

void print_centered_line(const char *text, int left_pad)
{
    for (int i = 0; i < left_pad; i++) {
        printf(" ");
    }

    printf("%s\n", text);
    return;
}

void print_centered(const char *text)
{
    char *textBuffer = malloc(strlen(text) + 1);
    check_mem(textBuffer);
    strcpy(textBuffer, text);

    // Need to match padding on all lines for alignment
    // If the text is multiline, find the longest line
    // and pad all other lines to match
    char *line = strtok(textBuffer, "\n");
    int max_length = 0;
    while (line != NULL) {
        int length = strlen(line);
        if (length > max_length) {
            max_length = length;
        }
        line = strtok(NULL, "\n");
    }

    int left_pad = (80 - max_length) / 2;
    strcpy(textBuffer, text);

    // If the text is multiline, print each line centered
    line = strtok(textBuffer, "\n");
    while (line != NULL) {
        print_centered_line(line, left_pad);
        line = strtok(NULL, "\n");
    }

    free(textBuffer);
    return;

error:
    free(textBuffer);
    return;
}



void print_status_bar_left(unsigned short health, unsigned short max_health, unsigned short mana, unsigned short max_mana)
{
    set_text_bold();
    set_text_white();
    printf("Health: ");
    set_text_green();
    set_text_normal();
    printf("%d/%d ", health, max_health);
    set_text_white();
    set_text_bold();
    printf("  Mana: ");
    set_text_blue();
    set_text_normal();
    printf("%d/%d", mana, max_mana);
    return;
}

void print_status_bar_center(char *current_location)
{
    set_text_bold();
    set_text_white();
    printf("%s", current_location);
    return;
}

void print_status_bar_right(unsigned short level, unsigned short experience, unsigned short experience_to_next_level)
{
    // Need length of text so we can right-align it
    char *buffer = malloc(80);
    check_mem(buffer);
    sprintf(buffer, "Level: %d  XP: %d/%d", level, experience, experience_to_next_level);
    int length = strlen(buffer);
    set_cursor_to_line_position(80 - length);

    set_text_bold();
    set_text_white();
    printf("Level: ");
    set_text_yellow();
    set_text_normal();
    printf("%d ", level);
    set_text_white();
    set_text_bold();
    printf("  XP: ");
    set_text_yellow();
    set_text_normal();
    printf("%d/%d", experience, experience_to_next_level);
    return;

error:
    return;
}

int calculate_status_bar_center_offset(struct Character *player, struct Location *current_location)
{
    int total_length = 80;
    char *buffer = malloc(total_length);
    check_mem(buffer);

    sprintf(buffer, "Health: %d/%d  Mana: %d/%d", player->health, player->max_health, player->mana, player->max_mana);

    int left_length = strlen(buffer);

    memset(buffer, 0, total_length);
    sprintf(buffer, "Level: %d  XP: %d/%d", player->level, (int)player->experience, player->level * player->level * 100);
    int right_length = strlen(buffer);

    int length = left_length + right_length;
    int free_length = total_length - length;
    int free_padding = free_length - strlen(current_location->name);
    check(free_padding > 0, "Status bar too big");

    free(buffer);

    return free_padding / 2 + left_length;

error:
    return -1;
}

void print_status_bar(struct Character *player, struct Location *current_location)
{
    // Remember where we were
    int row, col;
    get_cursor_position(&col, &row);

    int ctr_offset = calculate_status_bar_center_offset(player, current_location);
    set_cursor_to_screen_bottom();
    print_status_bar_left(player->health, player->max_health, player->mana, player->max_mana);

    if (ctr_offset > 0) {
        set_cursor_to_line_position(ctr_offset);
        print_status_bar_center(current_location->name);
    }

    print_status_bar_right(player->level, player->experience, player->level * player->level * 100);

    // Restore cursor position
    set_cursor_to_col_row(col, row);
    return;
}
