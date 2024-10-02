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
    frame->rawTextRepresentation = malloc(MAX_TEXT_SIZE);
    check_mem(frame->rawTextRepresentation);
    memset(frame->rawTextRepresentation, 0, MAX_TEXT_SIZE);

    // Make sure to reset defaults
    strncpy(frame->rawTextRepresentation, "\033[0m", 5);
    // Set to white
    strncat(frame->rawTextRepresentation, "\033[37m", 5);
    // Set null terminator
    frame->rawTextRepresentation[MAX_TEXT_SIZE - 1] = '\0';

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
            free(frame->rawTextRepresentation);
        }
        free(frame);
    }
}

void TS_print(struct TerminalSegment *frame)
{
    printf("%s", frame->rawTextRepresentation);
}

struct TerminalSegment *TS_clone(struct TerminalSegment *frame)
{
    struct TerminalSegment *clone = malloc(sizeof(struct TerminalSegment));
    check_mem(clone);
    clone->rawTextRepresentation = malloc(MAX_TEXT_SIZE);
    check_mem(clone->rawTextRepresentation);
    strncpy(clone->rawTextRepresentation, frame->rawTextRepresentation, MAX_TEXT_SIZE - 1);
    clone->rawTextRepresentation[MAX_TEXT_SIZE - 1] = '\0';
    clone->cursorCol = frame->cursorCol;
    clone->cursorRow = frame->cursorRow;
    return clone;

error:
    return NULL;
}

struct TerminalSegment *TS_concatText(struct TerminalSegment *frame, const char *text)
{
    // Need to check if text contains any newlines
    // If it does, we need to split the text into lines
    // and update the cursor position
    if (strchr(text, '\n') != NULL) {
        // Make a working copy of the text
        char *workingText = malloc(strlen(text) + 1);
        check_mem(workingText);
        strncpy(workingText, text, strlen(text) + 1);
        // Split text into lines and update cursor position
        char *line = strtok(workingText, "\n");
        while (line != NULL) {
            strcat(frame->rawTextRepresentation, line);
            strcat(frame->rawTextRepresentation, "\n");
            frame->cursorRow++;
            frame->cursorCol = 1 + strlen(line);
            line = strtok(NULL, "\n");
        }
        free(workingText);
        return frame;
    } else {
        // Is the text too long to fit on the current line?
        if (frame->cursorCol + strlen(text) > 80) {
            // If so, we need to wrap the text
            // First, we need a buffer
            char *lineBuffer = malloc(80);
            check_mem(lineBuffer);
            // Copy the text into the buffer
            strcpy(lineBuffer, text);
            // Find the last space
            char *lastSpace = strrchr(lineBuffer, ' ');
            // If there is no space, we need to split the word
            if (lastSpace == NULL) {
                // We'll just split the word in half
                int split = strlen(lineBuffer) / 2;
                // Find the first space
                char *firstSpace = strchr(lineBuffer, ' ');
                // If there is no space, we'll just split the word in half
                if (firstSpace == NULL) {
                    firstSpace = lineBuffer + split;
                }
                // Split the word
                *firstSpace = '\0';
                // Concatenate the first half of the word
                strcat(frame->rawTextRepresentation, lineBuffer);
                // Move to the next line
                strcat(frame->rawTextRepresentation, "\n");
                frame->cursorRow++;
                frame->cursorCol = 1;
                // Concatenate the second half of the word
                strcat(frame->rawTextRepresentation, firstSpace + 1);
                frame->cursorCol += strlen(firstSpace + 1);
            } else {
                // If there is a space, we'll just split the line there
                *lastSpace = '\0';
                strcat(frame->rawTextRepresentation, lineBuffer);
                strcat(frame->rawTextRepresentation, "\n");
                frame->cursorRow++;
                frame->cursorCol = 1;
                strcat(frame->rawTextRepresentation, lastSpace + 1);
                frame->cursorCol += strlen(lastSpace + 1);
            }
            free(lineBuffer);
        } else {
            // If the text fits on the current line, we can just concatenate it
            strcat(frame->rawTextRepresentation, text);
            frame->cursorCol += strlen(text);
        }
    }

    TS_calculate_cursor_position_from_raw(frame);
    return frame;

error:
    return NULL;
}

struct TerminalSegment *TS_setBold(struct TerminalSegment *frame)
{
    // Check if frame is empty
    if (TS_isEmpty(frame)) {
        // Have to strcpy bold code
        strcpy(frame->rawTextRepresentation, "\033[1m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[1m");
    }
    return frame;
}

struct TerminalSegment *TS_setDim(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[2m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[2m");
    }
    return frame;
}

struct TerminalSegment *TS_setUnderlined(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[4m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[4m");
    }
    return frame;
}

struct TerminalSegment *TS_setBlink(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[5m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[5m");
    }
    return frame;
}

struct TerminalSegment *TS_setNormal(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[0m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[0m");
    }
    return frame;
}

struct TerminalSegment *TS_setGreen(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[32m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[32m");
    }
    return frame;
}

struct TerminalSegment *TS_setWhite(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[37m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[37m");
    }
    return frame;
}

struct TerminalSegment *TS_setRed(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[31m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[31m");
    }
    return frame;
}

struct TerminalSegment *TS_setYellow(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[33m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[33m");
    }
    return frame;
}

struct TerminalSegment *TS_setBlue(struct TerminalSegment *frame)
{
    if (TS_isEmpty(frame)) {
        strcpy(frame->rawTextRepresentation, "\033[34m");
    } else {
        strcat(frame->rawTextRepresentation, "\033[34m");
    }
    return frame;
}

struct TerminalSegment *TS_setCentered(struct TerminalSegment *frame)
{
    // We're going to essentially rewrite the whole buffer.
    char *newBuffer = malloc(MAX_TEXT_SIZE);
    check_mem(newBuffer);
    memset(newBuffer, 0, MAX_TEXT_SIZE);

    // How many lines are currently in the frame?
    // Use a working copy of the text
    char *workingText = malloc(MAX_TEXT_SIZE);
    check_mem(workingText);
    strncpy(workingText, frame->rawTextRepresentation, MAX_TEXT_SIZE - 1);
    workingText[MAX_TEXT_SIZE - 1] = '\0';
    // Count the number of lines
    int lineCount = 0;
    char *line = strtok(workingText, "\n");
    while (line != NULL) {
        lineCount++;
        line = strtok(NULL, "\n");
    }
    free(workingText);

    if (lineCount == 0) {
        // If there are no lines, we don't need to do anything
        free(newBuffer);
        return frame;
    }
    if (lineCount == 1) {
        // If there's only one line, we can center it
        // Count the number of characters, but skip control codes
        int charCount = 0;
        for (size_t i = 0; i < strlen(frame->rawTextRepresentation); i++) {
            if (frame->rawTextRepresentation[i] == '\033') {
                // Skip control codes
                while (frame->rawTextRepresentation[i] != 'm') {
                    i++;
                }
            } else {
                charCount++;
            }
        }
        // Calculate the left padding
        int leftPad = (80 - charCount) / 2;
        // Format the line
        char *centeredLine = format_centered_line(frame->rawTextRepresentation, leftPad);
        // Copy the line into the new buffer
        strcpy(newBuffer, centeredLine);
        free(centeredLine);
    } else {
        // If there are multiple lines, we need to find the longest
        // line and center all the lines to that length
        int longestLine = 0;
        // Find the longest line
        // Use working buffer
        workingText = malloc(MAX_TEXT_SIZE);
        check_mem(workingText);
        strncpy(workingText, frame->rawTextRepresentation, MAX_TEXT_SIZE - 1);
        workingText[MAX_TEXT_SIZE - 1] = '\0';

        char *line = strtok(workingText, "\n");
        while (line != NULL) {
            int lineLength = 0;
            for (size_t i = 0; i < strlen(line); i++) {
                if (line[i] == '\033') {
                    // Skip control codes
                    while (line[i] != 'm') {
                        i++;
                    }
                } else {
                    lineLength++;
                }
            }
            if (lineLength > longestLine) {
                longestLine = lineLength;
            }
            line = strtok(NULL, "\n");
        }
        free(workingText);

        // Calculate the left padding for each line
        workingText = malloc(MAX_TEXT_SIZE);
        check_mem(workingText);
        strncpy(workingText, frame->rawTextRepresentation, MAX_TEXT_SIZE - 1);
        workingText[MAX_TEXT_SIZE - 1] = '\0';

        line = strtok(workingText, "\n");
        while (line != NULL) {
            int leftPad = (80 - longestLine) / 2;
            // Format the line
            char *centeredLine = format_centered_line(line, leftPad);
            // Concatenate the line into the new buffer
            strcat(newBuffer, centeredLine);
            strcat(newBuffer, "\n");
            free(centeredLine);
            line = strtok(NULL, "\n");
        }
    }

    // Clear the old frame
    memset(frame->rawTextRepresentation, 0, MAX_TEXT_SIZE);
    // Copy the new buffer into the frame
    strncpy(frame->rawTextRepresentation, newBuffer, MAX_TEXT_SIZE);
    // Make sure to add a null terminator
    frame->rawTextRepresentation[MAX_TEXT_SIZE - 1] = '\0';
    free(newBuffer);

    // Recalculate cursor position
    TS_calculate_cursor_position_from_raw(frame);
    return frame;

error:
    return NULL;
}

struct TerminalSegment *TS_setCursorPosition(struct TerminalSegment *frame, int col, int row)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%d;%dH", row, col);
    strcat(frame->rawTextRepresentation, position);
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
    strcat(frame->rawTextRepresentation, "\033[2K");
    // Reset cursor to beginning of line
    strcat(frame->rawTextRepresentation, "\033[1G");
    frame->cursorCol = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToLineStart(struct TerminalSegment *frame)
{
    strcat(frame->rawTextRepresentation, "\033[1G");
    frame->cursorCol = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToLinePosition(struct TerminalSegment *frame, int index)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%dG", index);
    strcat(frame->rawTextRepresentation, position);
    free(position);
    frame->cursorCol = index;
    return frame;

error:
    free(position);
    return NULL;
}

struct TerminalSegment *TS_setCursorToScreenTop(struct TerminalSegment *frame)
{
    strcat(frame->rawTextRepresentation, "\033[1;1H");
    frame->cursorCol = 1;
    frame->cursorRow = 1;
    return frame;
}

struct TerminalSegment *TS_setCursorToScreenBottom(struct TerminalSegment *frame)
{
    strcat(frame->rawTextRepresentation, "\033[24;1H");
    frame->cursorCol = 1;
    frame->cursorRow = 24;
    return frame;
}

struct TerminalSegment *TS_setCursorToScreenCenter(struct TerminalSegment *frame)
{
    strcat(frame->rawTextRepresentation, "\033[12;40H");
    frame->cursorCol = 40;
    frame->cursorRow = 12;
    return frame;
}

struct TerminalSegment *TS_setCursorToRow(struct TerminalSegment *frame, int row)
{
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%d;1H", row);
    strcat(frame->rawTextRepresentation, position);
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
    strcat(frame->rawTextRepresentation, "\033[2J");
    // Reset cursor to top of screen
    strcat(frame->rawTextRepresentation, "\033[1;1H");
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
        strcpy(dest->rawTextRepresentation, src->rawTextRepresentation);
        dest->cursorCol = src->cursorCol;
        dest->cursorRow = src->cursorRow;
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(src)) {
        // If the source is empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    }
    strncat(dest->rawTextRepresentation, "\n", 1); // Separate the two segments
    strncat(dest->rawTextRepresentation, src->rawTextRepresentation, MAX_TEXT_SIZE - strlen(dest->rawTextRepresentation) - 1);
    // Have to "add" cursor positions too
    // All segments consider themselves as starting at 1,1
    // So if dest has a cursor position of 1,1, and src has a cursor position of 1,1,
    // the cursor position of the appended segment should be 2,1
    dest->cursorRow += src->cursorRow;
    dest->cursorCol = src->cursorCol;
    TS_destroy(src);
    dest->rawTextRepresentation[MAX_TEXT_SIZE - 1] = '\0';
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
        strcpy(dest->rawTextRepresentation, src->rawTextRepresentation);
        dest->cursorCol = src->cursorCol;
        dest->cursorRow = src->cursorRow;
        TS_destroy(src);
        return dest;
    } else if (TS_isEmpty(src)) {
        // If the source is empty, we don't need to do anything
        TS_destroy(src);
        return dest;
    }
    strncat(dest->rawTextRepresentation, src->rawTextRepresentation, MAX_TEXT_SIZE - strlen(dest->rawTextRepresentation) - 1);
    // Have to "add" cursor positions too
    // All segments consider themselves as starting at 1,1
    // So if dest has a cursor position of 1,1, and src has a cursor position of 1,1,
    // the cursor position of the appended segment should be 2,1
    dest->cursorRow += src->cursorRow;
    dest->cursorCol = src->cursorCol;
    TS_destroy(src);
    dest->rawTextRepresentation[MAX_TEXT_SIZE - 1] = '\0';
    TS_calculate_cursor_position_from_raw(dest);
    return dest;
}

unsigned char TS_isEmpty(struct TerminalSegment *frame)
{
    // Don't count control codes
    for (size_t i = 0; i < strlen(frame->rawTextRepresentation) - 1; i++) {
        if (frame->rawTextRepresentation[i] == '\033') {
            // Skip control codes
            while (
                frame->rawTextRepresentation[i] != 'm' 
                && frame->rawTextRepresentation[i] != 'J'
                && frame->rawTextRepresentation[i] != 'K'
                && frame->rawTextRepresentation[i] != 'H'
                && frame->rawTextRepresentation[i] != 'G'
            ) {
                i++;
            }
        } else {
            return 0;
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
    
    // We need a working copy of the raw text representation
    char *workingText = malloc(MAX_TEXT_SIZE);
    check_mem(workingText);
    strncpy(workingText, frame->rawTextRepresentation, MAX_TEXT_SIZE - 1);
    workingText[MAX_TEXT_SIZE - 1] = '\0';

    // We need to reset the cursor position
    frame->cursorCol = 1;
    frame->cursorRow = 1;

    // Walk through the working text
    for (size_t i = 0; i < strlen(workingText); i++) {
        if (workingText[i] != '\033') {
            // Proceed normally
            if (workingText[i] == '\n') {
                frame->cursorRow++;
                frame->cursorCol = 1;
            } else {
                frame->cursorCol++;
                if (frame->cursorCol > 80) {
                    frame->cursorCol = 1;
                    frame->cursorRow++;
                }
            }
        } else {
            // Gather control code arguments
            char *controlCode = malloc(20);
            check_mem(controlCode);
            memset(controlCode, 0, 20);
            i++;
            while (workingText[i] != 'm' && workingText[i] != 'H' && workingText[i] != 'G') {
                strncat(controlCode, workingText + i, 1);
                i++;
            }
            // Positioning control codes:
            // Check for [row;colH
            if (strstr(controlCode, ";") != NULL) {
                char *row = strtok(controlCode, ";");
                char *col = strtok(NULL, ";");
                frame->cursorRow = atoi(row);
                frame->cursorCol = atoi(col);
            } else {
                // Check for [rowG
                if (controlCode[strlen(controlCode) - 1] == 'G') {
                    frame->cursorCol = atoi(controlCode);
                } 
            }
            free(controlCode);
        }
    }

    free(workingText);
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
    char *position = malloc(20);
    check_mem(position);
    sprintf(position, "\033[%d;1H", row);
    char *positionStart = strstr(frame->rawTextRepresentation, "\033[");

    if (positionStart != NULL) {
        // Find the end of the control code
        char *positionEnd = strchr(positionStart, 'H');
        // Calculate the length of the control code
        int positionLength = positionEnd - positionStart + 1;
        // Replace the control code with the new control code
        strncpy(positionStart, position, positionLength);
    } else {
        // Append the control code to the beginning of the raw representation
        char *newBuffer = malloc(MAX_TEXT_SIZE);
        check_mem(newBuffer);
        memset(newBuffer, 0, MAX_TEXT_SIZE);
        strcat(newBuffer, position);
        strcat(newBuffer, frame->rawTextRepresentation);
        strncpy(frame->rawTextRepresentation, newBuffer, MAX_TEXT_SIZE);
        free(newBuffer);
    }

    free(position);
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

    struct TerminalSegment *display = TS_new();
    struct TerminalSegment *header_backup = TS_clone(state->header);

    check_mem(display);
    display = TS_appendInline(display, state->header); // Remember, calling TS_append will destroy the source segment
    state->header = header_backup;

    if (TS_isEmpty(state->text)) {
        display = TS_appendInline(display, state->text);
    } else {
        display = TS_append(display, state->text);
    }
    int textRow = display->cursorRow + 1;
    state->text = TS_setCursorToRow(TS_new(), state->header->cursorRow + 1);

    display = TS_appendInline(display, state->statusBar); // Since status bar sets itself to the bottom of the screen, we can just append it inline
    state->statusBar = TS_setCursorToScreenBottom(TS_new());

    TS_setCursorToRow(display, textRow);

    strncpy(buffer, display->rawTextRepresentation, MAX_TEXT_SIZE);

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

void ScreenState_headerAppend(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_append(state->header, segment);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_headerAppendInline(struct ScreenState *state, struct TerminalSegment *segment)
{
    TS_appendInline(state->header, segment);
    TS_presetCursorToRow(state->text, state->header->cursorRow + 1);
}

void ScreenState_textSet(struct ScreenState *state, const char *text)
{
    TS_destroy(state->text);
    state->text = TS_concatText(TS_new(), text);
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