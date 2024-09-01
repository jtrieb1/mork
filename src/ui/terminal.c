#include "terminal.h"

#include <stdio.h>

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
