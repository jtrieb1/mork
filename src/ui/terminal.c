#include "terminal.h"

#include <lcthw/dbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

const unsigned int MAX_TEXT_SIZE = 2048;
const unsigned int MAX_LINE_SIZE_WITH_CODES = 120;

char *format_centered_line(const char *text, int left_pad)
{
    char *lineBuffer = malloc(MAX_LINE_SIZE_WITH_CODES);
    check_mem(lineBuffer);
    memset(lineBuffer, 0, MAX_LINE_SIZE_WITH_CODES);

    for (int i = 0; i < left_pad; i++) {
        strncat(lineBuffer, " ", 1);
    }

    strcat(lineBuffer, text);
    lineBuffer[MAX_LINE_SIZE_WITH_CODES - 1] = '\0';
    return lineBuffer;

error:
    return NULL;
}

struct TerminalSegment *TS_new()
{
    struct TerminalSegment *frame = malloc(sizeof(struct TerminalSegment));
    check_mem(frame);
    
    // Even though our terminal is only 80 characters wide,
    // we need to allow space for control codes.
    frame->rawTextRepresentation = bfromcstr("\033[0m");
    check_mem(frame->rawTextRepresentation);

    frame->cursorCol = 1;
    frame->cursorRow = 1;
    return frame;

error:
    return NULL;
}

void TS_destroy(struct TerminalSegment *frame)
{
    if (frame) {
        if (frame->rawTextRepresentation != NULL) {
            bdestroy(frame->rawTextRepresentation);
            frame->rawTextRepresentation = NULL;
        }
        free(frame);
    }
}

void TS_print(struct TerminalSegment *frame)
{
    printf("%s", frame->rawTextRepresentation->data);
}

struct TerminalSegment *TS_clone(struct TerminalSegment *frame)
{
    struct TerminalSegment *clone = malloc(sizeof(struct TerminalSegment));
    check_mem(clone);
    if (frame->rawTextRepresentation != NULL) {
        clone->rawTextRepresentation = bstrcpy(frame->rawTextRepresentation);
    } else {
        clone->rawTextRepresentation = NULL;
    }

    clone->cursorCol = frame->cursorCol;
    clone->cursorRow = frame->cursorRow;
    return clone;

error:
    return NULL;
}

struct TerminalSegment *TS_concatText(struct TerminalSegment *frame, const char *text)
{
    bstring newText = bfromcstr(text);
    check_mem(newText);

    bconcat(frame->rawTextRepresentation, newText);
    bdestroy(newText);

    TS_calculate_cursor_position_from_raw(frame);
    return frame;

error:
    return NULL;
}

struct TerminalSegment *TS_setBold(struct TerminalSegment *frame)
{
    // Check if frame is empty
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[1m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[1m");
    }
    return frame;
}

struct TerminalSegment *TS_setDim(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[2m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[2m");
    }
    return frame;
}

struct TerminalSegment *TS_setUnderlined(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[4m"); 
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[4m");
    }
    return frame;
}

struct TerminalSegment *TS_setBlink(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[5m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[5m");
    }
    return frame;
}

struct TerminalSegment *TS_setNormal(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[0m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[0m");
    }
    return frame;
}

struct TerminalSegment *TS_setGreen(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[32m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[32m");
    }
    return frame;
}

struct TerminalSegment *TS_setWhite(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[37m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[37m");
    }
    return frame;
}

struct TerminalSegment *TS_setRed(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[31m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[31m");
    }
    return frame;
}

struct TerminalSegment *TS_setYellow(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[33m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[33m");
    }
    return frame;
}

struct TerminalSegment *TS_setBlue(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        bassigncstr(frame->rawTextRepresentation, "\033[34m");
    } else {
        bcatcstr(frame->rawTextRepresentation, "\033[34m");
    }
    return frame;
}

struct TerminalSegment *TS_setCentered(struct TerminalSegment *frame)
{
    // We need to find the longest line in the raw text representation
    // and calculate the padding needed to center the text
    int longestLine = 0;
    int currentLine = 0;
    int currentLength = 0;

    for (int i = 0; i < blength(frame->rawTextRepresentation); i++) {
        if (bdata(frame->rawTextRepresentation)[i] == '\n') {
            if (currentLength > longestLine) {
                longestLine = currentLength;
            }
            currentLength = 0;
            currentLine++;
        } else {
            currentLength++;
        }
    }

    int left_pad = (80 - longestLine) / 2;
    // For each line, we need to add padding
    // Split into lines
    struct bstrList *lines = bsplits(frame->rawTextRepresentation, bfromcstr("\n"));
    check(lines != NULL, "Failed to split lines.");

    bstring buffer = bfromcstr("");

    for (int i = 0; i < lines->qty; ++i) {
        char *centeredLine = format_centered_line(bdata(lines->entry[i]), left_pad);
        check(centeredLine != NULL, "Failed to format centered line.");
        bconcat(buffer, bfromcstr(centeredLine));
        free(centeredLine);
    }

    bstrListDestroy(lines);
    bassign(frame->rawTextRepresentation, buffer);

error:
    return NULL;
}

struct TerminalSegment *TS_setCursorPosition(struct TerminalSegment *frame, int col, int row)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%d;%dH", row, col);
    bcatcstr(frame->rawTextRepresentation, position);
    free(position);
    frame->cursorCol = col;
    frame->cursorRow = row;
    return frame;

error:
    free(position);
    return NULL;
}

struct TerminalSegment *TS_clearLine(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[2K");
    // Reset cursor to beginning of line
    bcatcstr(frame->rawTextRepresentation, "\033[1G");
    frame->cursorCol = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToLineStart(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[1G");
    frame->cursorCol = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToLinePosition(struct TerminalSegment *frame, int index)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%dG", index);
    bcatcstr(frame->rawTextRepresentation, position);
    free(position);
    frame->cursorCol = index;
    return frame;

error:
    free(position);
    return NULL;
}

struct TerminalSegment *TS_setCursorToScreenTop(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[1;1H");
    frame->cursorCol = 1;
    frame->cursorRow = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToScreenBottom(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[24;1H");
    frame->cursorCol = 1;
    frame->cursorRow = 24;
    return frame;
}

struct TerminalSegment *TS_setCursorToScreenCenter(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[12;40H");
    frame->cursorCol = 40;
    frame->cursorRow = 12;
    return frame;
}

struct TerminalSegment *TS_setCursorToRow(struct TerminalSegment *frame, int row)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%d;1H", row);
    bcatcstr(frame->rawTextRepresentation, position);
    free(position);
    frame->cursorCol = 1;
    frame->cursorRow = row;
    return frame;

error:
    free(position);
    return NULL;
}

struct TerminalSegment *TS_clearScreen(struct TerminalSegment *frame)
{
    bcatcstr(frame->rawTextRepresentation, "\033[2J");
    // Reset cursor to top of screen
    bcatcstr(frame->rawTextRepresentation, "\033[1;1H");
    frame->cursorCol = 1;
    frame->cursorRow = 1;
    return frame;
}

struct TerminalSegment *TS_append(struct TerminalSegment *dest, struct TerminalSegment *src)
{
    if (dest == NULL) {
        return src;
    } else if (src == NULL) {
        return dest;
    }

    if (TS_isEmpty(dest) && TS_isEmpty(src)) {
        // If both segments are empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(dest)) {
        // If the destination is empty, we can just copy the source
        bassign(dest->rawTextRepresentation, src->rawTextRepresentation);
        dest->cursorCol = src->cursorCol;
        dest->cursorRow = src->cursorRow;
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(src)) {
        // If the source is empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    }
    bcatcstr(dest->rawTextRepresentation, "\n"); // Separate the two segments
    bcatcstr(dest->rawTextRepresentation, bdata(src->rawTextRepresentation));
    
    TS_destroy(src);
    TS_calculate_cursor_position_from_raw(dest);
    return dest;
}

struct TerminalSegment *TS_appendInline(struct TerminalSegment *dest, struct TerminalSegment *src)
{
    // Same as TS_append but without a newline inbetween
    if (dest == NULL) {
        return src;
    } else if (src == NULL) {
        return dest;
    }

    if (TS_isEmpty(dest) && TS_isEmpty(src)) {
        // If both segments are empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(dest)) {
        // If the destination is empty, we can just copy the source
        bassign(dest->rawTextRepresentation, src->rawTextRepresentation);
        dest->cursorCol = src->cursorCol;
        dest->cursorRow = src->cursorRow;
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(src)) {
        // If the source is empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    }
    bcatcstr(dest->rawTextRepresentation, bdata(src->rawTextRepresentation));

    TS_destroy(src);
    TS_calculate_cursor_position_from_raw(dest);
    return dest;
}

unsigned char TS_isEmpty(struct TerminalSegment *frame)
{
    if (frame == NULL || frame->rawTextRepresentation == NULL) {
        return 1;
    }
    // Don't count control codes
    for (int i = 0; i < blength(frame->rawTextRepresentation); ++i) {
        if (bchar(frame->rawTextRepresentation, i) != '\033') {
            return 0;
        } else {
            // Skip control code
            while (bchar(frame->rawTextRepresentation, i) != 'm' && bchar(frame->rawTextRepresentation, i) != 'H') {
                i++;
            }
        }
    }
    return 1;
}

unsigned char TS_getCursorPosition(struct TerminalSegment *frame, int *col, int *row)
{
    // We don't need to ask the terminal, just keep track of it
    *col = frame->cursorCol;
    *row = frame->cursorRow;
    return 1;
}

void TS_calculate_cursor_position_from_raw(struct TerminalSegment *frame)
{
    // We need to scan the raw text representation
    // for control codes that set the cursor position
    // and update the cursor position accordingly
    
    // We'll start at the beginning of the raw text representation
    for (int i = 0; i < blength(frame->rawTextRepresentation); ++i) {
        if (bchar(frame->rawTextRepresentation, i) != '\033') {
            // Normal, not control
            // Check for newline
            if (bchar(frame->rawTextRepresentation, i) == '\n') {
                frame->cursorCol = 1;
                frame->cursorRow++;
            } else {
                frame->cursorCol++;
                if (frame->cursorCol > 80) {
                    frame->cursorCol = 1;
                    frame->cursorRow++;
                }
            }
        } else {
            // Control code
            // We need to find the end of the control code
            int j = i;
            while (bchar(frame->rawTextRepresentation, j) != 'm' && bchar(frame->rawTextRepresentation, j) != 'H') {
                j++;
            }
            // We need to check if the control code is a cursor positioning code
            if (bchar(frame->rawTextRepresentation, j) == 'H') {
                // Cursor positioning code
                // We need to extract the row and column
                int k = i + 2;
                int col = 0;
                while (bchar(frame->rawTextRepresentation, k) != ';') {
                    col = col * 10 + (bchar(frame->rawTextRepresentation, k) - '0');
                    k++;
                }
                int row = 0;
                k++;
                while (bchar(frame->rawTextRepresentation, k) != 'H') {
                    row = row * 10 + (bchar(frame->rawTextRepresentation, k) - '0');
                    k++;
                }
                frame->cursorCol = col;
                frame->cursorRow = row;
            }
            // Move the index to the end of the control code
            i = j + 1;
        }
    }

    return;

error:
    return;
}

struct TerminalSegment *TS_presetCursorToRow(struct TerminalSegment *frame, int row)
{
    // Scan the beginning of the raw text representation
    // for a positioning control code. If one is found,
    // replace it with the new positioning control code.
    // Otherwise, append the new positioning control code
    // to the beginning of the raw representation.
    bstring position = bformat("\033[%d;1H", row);
    
    // Check to see if we have a control code at the beginning
    if (bchar(frame->rawTextRepresentation, 0) == '\033') {
        // We have a control code at the beginning
        // We need to find the end of the control code
        int i = 0;
        while (bchar(frame->rawTextRepresentation, i) != 'm' && bchar(frame->rawTextRepresentation, i) != 'H') {
            i++;
        }
        // We need to check if the control code is a cursor positioning code
        if (bchar(frame->rawTextRepresentation, i) == 'H') {
            // Cursor positioning code
            // We don't care what it is, replace with position
            bdelete(frame->rawTextRepresentation, 0, i);
            binsert(frame->rawTextRepresentation, 0, position, ' ');
        } else {
            // Not a cursor positioning code, insert position
            binsert(frame->rawTextRepresentation, 0, position, ' ');
        }
    }

    bdestroy(position);
    // Now we have to recalculate the cursor position
    TS_calculate_cursor_position_from_raw(frame);
    return frame;

error:
    free(position);
    return NULL;
}

struct ScreenState *ScreenState_create()
{
    struct ScreenState *state = malloc(sizeof(struct ScreenState));
    check_mem(state);

    state->header = TS_new();
    state->text = TS_setCursorToRow(TS_new(), state->header->cursorRow + 1);
    state->statusBar = TS_setCursorToScreenBottom(TS_new());

    return state;

error:
    return NULL;
}

void ScreenState_destroy(struct ScreenState *state)
{
    if (state) {
        if (state->header) {
            TS_destroy(state->header);
        }
        if (state->text) {
            TS_destroy(state->text);
        }
        if (state->statusBar) {
            TS_destroy(state->statusBar);
        }

        free(state);
    }
}

char *ScreenState_getDisplay(struct ScreenState *state)
{
    if (state == NULL) {
        return NULL;
    }

    char *buffer = malloc(MAX_TEXT_SIZE * 3);
    check_mem(buffer);
    memset(buffer, 0, MAX_TEXT_SIZE * 3);

    struct TerminalSegment *display = TS_new();
    struct TerminalSegment *header_backup = TS_clone(state->header);
    struct TerminalSegment *text_backup = TS_clone(state->text);
    struct TerminalSegment *statusBar_backup = TS_clone(state->statusBar);

    check_mem(display);
    display = TS_appendInline(display, state->header); // Remember, calling TS_append will destroy the source segment
    state->header = header_backup;

    if (TS_isEmpty(state->text)) {
        display = TS_appendInline(display, state->text);
    } else {
        display = TS_append(display, state->text);
    }
    int textRow = display->cursorRow + 1;
    state->text = text_backup;

    display = TS_appendInline(display, state->statusBar); // Since status bar sets itself to the bottom of the screen, we can just append it inline
    state->statusBar = statusBar_backup;

    TS_setNormal(display);
    TS_setCursorToRow(display, textRow);

    sprintf(buffer, "%s", display->rawTextRepresentation->data);

    TS_destroy(display);
    return buffer;

error:
    return NULL;
}

void ScreenState_print(struct ScreenState *state)
{
    char *display = ScreenState_getDisplay(state);
    printf("%s", display);
    free(display);
}

void ScreenState_clear()
{
    TS_print(TS_clearScreen(TS_new()));
}

void ScreenState_headerSet(struct ScreenState *state, const char *text)
{
    TS_destroy(state->header);
    state->header = TS_concatText(TS_new(), text);
    check(state->header != NULL, "Failed to set header.");

    if (state->text == NULL) {
        state->text = TS_setCursorToRow(TS_new(), state->header->cursorRow + 1);
    } else if (TS_isEmpty(state->text)) {
        TS_destroy(state->text);
        state->text = TS_setCursorToRow(TS_new(), state->header->cursorRow + 1);
    } else {
        TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
    }
error:
    return;
}

void ScreenState_headerReplace(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_destroy(state->header);
    state->header = segment;
    TS_calculate_cursor_position_from_raw(state->header);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_headerAppend(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_append(state->header, segment);
    TS_calculate_cursor_position_from_raw(state->header);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_headerAppendInline(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_appendInline(state->header, segment);
    TS_calculate_cursor_position_from_raw(state->header);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_textSet(struct ScreenState *state, const char *text)
{
    TS_destroy(state->text);
    state->text = TS_concatText(TS_new(), text);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_textReplace(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_destroy(state->text);
    state->text = segment;
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_textAppend(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_append(state->text, segment);
}

void ScreenState_textAppendInline(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_appendInline(state->text, segment);
}

void ScreenState_statusBarSet(struct ScreenState *state, const char *text)
{
    TS_destroy(state->statusBar);
    state->statusBar = TS_concatText(TS_setCursorToScreenBottom(TS_new()), text);
}

void ScreenState_statusBarAppend(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_append(state->statusBar, segment);
}

void ScreenState_statusBarAppendInline(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_appendInline(state->statusBar, segment);
}