#include "../inc/window.h"
#include <X11/Xlib.h>
#include <stdio.h>
#define MAX_LINES 50
#define MAX_CHARS_PER_LINE 256
#define X_LINES_OFFSET 20
#define Y_LINES_OFFSET 12

const char *parseSpecialKeysyms(const char *keysym);
char *getKeysymToString(XKeyEvent *xkey);
char *getOutputFromCommandFile(char *command, char *buffer);
FILE *returnOutputOfRanCommand(char *command);
char *returnCommandOutput(FILE *output, char *outputBuffer);
void printCommandOutput(struct DisplayWindowContext displayWindowContext,
                        struct WindowProperties windowProperties,
                        char *command);
char *runCommand(char *command);
void splitIntoArrayOfLines(char *outputBuffer,
                           char lines[][MAX_CHARS_PER_LINE]);
char *removeLastCharacter(char *text);
void drawString(struct DisplayWindowContext displayWindowContext, char *buffer);
void drawLines(struct DisplayWindowContext displayWindowContext,
               struct WindowProperties properties,
               char lines[][MAX_CHARS_PER_LINE]);
int isEscape(XEvent event);
int isSafeIncrementLines(int linesIndex);
int isSafeIncrementChars(int charInLineIndex);
