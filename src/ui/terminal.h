#include "../models/character.h"
#include "../models/location.h"

void clear_screen();

// Text style
void set_text_bold();
void set_text_dim();
void set_text_underlined();
void set_text_blink();
void set_text_normal();

// Text color
void set_text_green();
void set_text_white();
void set_text_red();
void set_text_yellow();
void set_text_blue();

// Positioning the cursor
void set_cursor_to_screen_top();
void set_cursor_to_screen_bottom();
void set_cursor_to_screen_center();
void set_cursor_to_line_start();
void set_cursor_to_line_position(int index);
void get_cursor_position(int *col, int *row);
void get_cursor_row(int *row);
void get_cursor_col(int *col);
void set_cursor_to_col_row(int col, int row);
void set_cursor_to_row(int line);

// Clearing the line
void clear_line();

// Composite actions
void refresh_screen();
void print_centered(const char *text);
void print_status_bar(struct Character *player, struct Location *current_location);

// Printing sections of the status bar
void print_status_bar_left(unsigned short health, unsigned short max_health, unsigned short mana, unsigned short max_mana);
void print_status_bar_center(char *current_location);
void print_status_bar_right(unsigned short level, unsigned short experience, unsigned short experience_to_next_level);

// Printing the prompt
void print_prompt(const char *prompt);
