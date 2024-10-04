#pragma once

#include <lcthw/bstrlib.h>

#include "../models/character.h"
#include "../models/location.h"

void clear_screen();

// We assume an 80 character wide screen with 24 lines
// We'll allow for a 1 character buffer on each side

// The header will be at most 5 lines, and 78 characters wide
struct TerminalSegment {
    int cursorCol;
    int cursorRow;
    bstring rawTextRepresentation;
};

void TS_calculate_cursor_position_from_raw(struct TerminalSegment *frame);

struct TerminalSegment *TS_new();
struct TerminalSegment *TS_clone(struct TerminalSegment *frame);
void TS_destroy(struct TerminalSegment *frame);
void TS_print(struct TerminalSegment *frame);
struct TerminalSegment *TS_concatText(struct TerminalSegment *frame, const char *text);
struct TerminalSegment *TS_setBold(struct TerminalSegment *frame);
struct TerminalSegment *TS_setDim(struct TerminalSegment *frame);
struct TerminalSegment *TS_setUnderlined(struct TerminalSegment *frame);
struct TerminalSegment *TS_setBlink(struct TerminalSegment *frame);
struct TerminalSegment *TS_setNormal(struct TerminalSegment *frame);
struct TerminalSegment *TS_setGreen(struct TerminalSegment *frame);
struct TerminalSegment *TS_setWhite(struct TerminalSegment *frame);
struct TerminalSegment *TS_setRed(struct TerminalSegment *frame);
struct TerminalSegment *TS_setYellow(struct TerminalSegment *frame);
struct TerminalSegment *TS_setBlue(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCentered(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorPosition(struct TerminalSegment *frame, int col, int row);
struct TerminalSegment *TS_clearLine(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorToLineStart(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorToLinePosition(struct TerminalSegment *frame, int index);
struct TerminalSegment *TS_setCursorToScreenTop(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorToScreenBottom(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorToScreenCenter(struct TerminalSegment *frame);
struct TerminalSegment *TS_setCursorToRow(struct TerminalSegment *frame, int row);
struct TerminalSegment *TS_append(struct TerminalSegment *dest, struct TerminalSegment *src);
struct TerminalSegment *TS_appendInline(struct TerminalSegment *dest, struct TerminalSegment *src);
struct TerminalSegment *TS_clearScreen(struct TerminalSegment *frame);

struct TerminalSegment *TS_presetCursorToRow(struct TerminalSegment *frame, int row);

unsigned char TS_isEmpty(struct TerminalSegment *frame);
unsigned char TS_getCursorPosition(struct TerminalSegment *frame, int *col, int *row);

struct ScreenState {
    struct TerminalSegment *header;
    struct TerminalSegment *text;
    struct TerminalSegment *statusBar;
};

struct ScreenState *ScreenState_create();
void ScreenState_destroy(struct ScreenState *state);
void ScreenState_print(struct ScreenState *state);
char *ScreenState_getDisplay(struct ScreenState *state);
void ScreenState_clear();

void ScreenState_headerSet(struct ScreenState *state, const char *text);
void ScreenState_headerReplace(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_headerAppend(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_headerAppendInline(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_textSet(struct ScreenState *state, const char *text);
void ScreenState_textReplace(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_textAppend(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_textAppendInline(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_statusBarSet(struct ScreenState *state, const char *text);
void ScreenState_statusBarAppend(struct ScreenState *state, struct TerminalSegment *segment);
void ScreenState_statusBarAppendInline(struct ScreenState *state, struct TerminalSegment *segment);