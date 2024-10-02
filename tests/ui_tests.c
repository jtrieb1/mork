#include "minunit.h"

#include "../src/ui/terminal.h"

char *test_create()
{
    struct TerminalSegment *frame = TS_new();
    mu_assert(TS_isEmpty(frame) == 1, "Failed to create empty terminal segment.");
    mu_assert(frame != NULL, "Failed to create terminal segment.");
    mu_assert(frame->cursorCol == 1, "Failed to set cursor column.");
    mu_assert(frame->cursorRow == 1, "Failed to set cursor row.");
    mu_assert(frame->rawTextRepresentation != NULL, "Failed to set raw text representation.");

    TS_destroy(frame);
    return NULL;
}

char *test_simple_TS_append()
{
    struct TerminalSegment *dest = TS_new();
    mu_assert(dest != NULL, "Failed to create destination terminal segment.");
    mu_assert(dest->cursorCol == 1, "Failed to set destination cursor column.");
    mu_assert(dest->cursorRow == 1, "Failed to set destination cursor row.");
    mu_assert(dest->rawTextRepresentation != NULL, "Failed to set destination raw text representation.");

    struct TerminalSegment *src = TS_new();
    mu_assert(src != NULL, "Failed to create source terminal segment.");
    mu_assert(src->cursorCol == 1, "Failed to set source cursor column.");
    mu_assert(src->cursorRow == 1, "Failed to set source cursor row.");
    mu_assert(src->rawTextRepresentation != NULL, "Failed to set source raw text representation.");

    TS_append(dest, src);
    mu_assert(dest->cursorCol == 1, "Failed to set destination cursor column after append.");
    mu_assert(dest->cursorRow == 1, "Failed to set destination cursor row after append.");
    mu_assert(dest->rawTextRepresentation != NULL, "Failed to set destination raw text representation after append.");

    TS_destroy(dest);
    return NULL;
}

char *test_TS_append_to_empty()
{
    struct TerminalSegment *dest = TS_new();
    mu_assert(dest != NULL, "Failed to create destination terminal segment.");
    mu_assert(dest->cursorCol == 1, "Failed to set destination cursor column.");
    mu_assert(dest->cursorRow == 1, "Failed to set destination cursor row.");
    mu_assert(dest->rawTextRepresentation != NULL, "Failed to set destination raw text representation.");

    struct TerminalSegment *src = TS_new();
    mu_assert(src != NULL, "Failed to create source terminal segment.");
    mu_assert(src->cursorCol == 1, "Failed to set source cursor column.");
    mu_assert(src->cursorRow == 1, "Failed to set source cursor row.");
    mu_assert(src->rawTextRepresentation != NULL, "Failed to set source raw text representation.");

    TS_concatText(src, "Hello, world!");
    TS_append(dest, src);
    mu_assert(dest->cursorCol == 14, "Failed to set destination cursor column after append.");
    mu_assert(dest->cursorRow == 1, "Failed to set destination cursor row after append.");
    mu_assert(strcmp(dest->rawTextRepresentation, "\033[0m\033[37mHello, world!") == 0, "Failed to set destination raw text representation after append.");

    TS_destroy(dest);
    return NULL;
}

char *test_TS_append_to_full()
{
    struct TerminalSegment *dest = TS_new();
    mu_assert(dest != NULL, "Failed to create destination terminal segment.");
    mu_assert(dest->cursorCol == 1, "Failed to set destination cursor column.");
    mu_assert(dest->cursorRow == 1, "Failed to set destination cursor row.");
    mu_assert(dest->rawTextRepresentation != NULL, "Failed to set destination raw text representation.");

    struct TerminalSegment *src = TS_new();
    mu_assert(src != NULL, "Failed to create source terminal segment.");
    mu_assert(src->cursorCol == 1, "Failed to set source cursor column.");
    mu_assert(src->cursorRow == 1, "Failed to set source cursor row.");
    mu_assert(src->rawTextRepresentation != NULL, "Failed to set source raw text representation.");

    TS_concatText(src, "Hello again!");
    TS_concatText(dest, "Hello, world!");
    TS_append(dest, src);
    mu_assert(dest->cursorCol == 13, "Failed to set destination cursor column after append.");
    mu_assert(dest->cursorRow == 2, "Failed to set destination cursor row after append.");
    mu_assert(strcmp(dest->rawTextRepresentation, "\033[0m\033[37mHello, world!\n\033[0m\033[37mHello again!") == 0, "Failed to set destination raw text representation after append.");

    TS_destroy(dest);
    return NULL;
}

char *test_create_screen()
{
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    ScreenState_destroy(state);
    return NULL;
}

char *test_screen_print()
{
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    ScreenState_print(state);

    ScreenState_destroy(state);
    return NULL;
}

char *test_screen_get_display_simple()
{
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    char *display = ScreenState_getDisplay(state);
    mu_assert(display != NULL, "Failed to get display.");

    free(display);
    ScreenState_destroy(state);
    return NULL;
}

char *test_screen_set_header()
{
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    ScreenState_headerSet(state, "Header");
    char *display = ScreenState_getDisplay(state);
    mu_assert(display != NULL, "Failed to get display.");
    log_info("Display: %s", display);
    mu_assert(strcmp(display, "\033[0m\033[37mHeader\033[2;1H") == 0, "Failed to set header.");

    free(display);
    ScreenState_destroy(state);
    return NULL;
}

char *test_screen_set_multiline_header()
{
    // Make sure text section starts on row after header
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    ScreenState_headerSet(state, "Header\nHeader");
    mu_assert(state->header->cursorRow == 3, "Failed to set header cursor row.");
    mu_assert(state->text->cursorRow == 4, "Failed to set text cursor row.");

    char *display = ScreenState_getDisplay(state);
    mu_assert(display != NULL, "Failed to get display.");
    log_info("Display: %s", display);
    mu_assert(strcmp(display, "\033[0m\033[37mHeader\nHeader\n\033[4;1H") == 0, "Failed to set header.");
    
    free(display);
    ScreenState_destroy(state);
    return NULL;
}

char *test_screen_set_text()
{
    struct ScreenState *state = ScreenState_create();
    mu_assert(state != NULL, "Failed to create screen state.");
    mu_assert(state->header != NULL, "Failed to create header terminal segment.");
    mu_assert(state->text != NULL, "Failed to create text terminal segment.");
    mu_assert(state->statusBar != NULL, "Failed to create status bar terminal segment.");

    ScreenState_textSet(state, "Text");
    char *display = ScreenState_getDisplay(state);
    mu_assert(display != NULL, "Failed to get display.");
    log_info("Display: %s", display);
    mu_assert(strcmp(display, "\033[0m\033[37m\033[24;1HText\n\033[0m\033[37m\033[3;1H") == 0, "Failed to set text.");

    free(display);
    ScreenState_destroy(state);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_simple_TS_append);
    mu_run_test(test_TS_append_to_empty);
    mu_run_test(test_TS_append_to_full);
    mu_run_test(test_create_screen);
    mu_run_test(test_screen_print);
    mu_run_test(test_screen_get_display_simple);
    mu_run_test(test_screen_set_header);
    mu_run_test(test_screen_set_multiline_header);

    return NULL;
}

RUN_TESTS(all_tests);
