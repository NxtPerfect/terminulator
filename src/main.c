#include "../inc/window.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 50
#define MAX_CHARS_PER_LINE 256

char *parseSpecialKeysyms(const char *keysym);
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
void drawString(struct DisplayWindowContext displayWindowContext,
                struct WindowProperties properties,
                char lines[][MAX_CHARS_PER_LINE]);
int isEscape(XEvent event);

char lineBuffers[MAX_LINES][MAX_CHARS_PER_LINE];
int indexOfLine = 0;

int main(int argc, char *argv[]) {
  struct DisplayWindowContext displayWindowContext;
  displayWindowContext.display = getDisplay();
  if (displayWindowContext.display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }

  displayWindowContext.window = createWindow(displayWindowContext.display);

  struct WindowProperties properties = createDefaultWindowProperties();
  displayWindowContext.gc =
      DefaultGC(displayWindowContext.display, properties.screenNumber);

  XEvent event;

  char buffer[MAX_CHARS_PER_LINE] = "";

  while (True) {
    XNextEvent(displayWindowContext.display, &event);

    if (event.type != KeyPress)
      continue;

    if (isEscape(event))
      break;

    // TODO: getKeysymToString(&event.xkey);
    KeySym keysym = XLookupKeysym(&event.xkey, 0);

    if (keysym == 0) {
      printf("Error looking up keysym %lu \n", keysym);
    }

    const char *keysymString = XKeysymToString(keysym);

    if (keysymString == NULL) {
      printf("Failed changing keysym to string.\n");
    }

    printf("Key pressed here: %s\n", keysymString);

    char *parsedKeysym = parseSpecialKeysyms(keysymString);

    // BackSpace
    if (!strcmp(parsedKeysym, "")) {
      removeLastCharacter(buffer);
    }

    // Enter
    char outputBuffer[4096];
    bool isCommandOutput = false;
    if (!strcmp(parsedKeysym, "\n")) {
      printCommandOutput(displayWindowContext, properties, buffer);
      isCommandOutput = true;
    }

    int draw = -1;
    if (isCommandOutput) {
      strcpy(buffer, "");
      draw = 0;
      isCommandOutput = false;
    } else {
      strcat(buffer, parsedKeysym);
      XClearWindow(displayWindowContext.display, displayWindowContext.window);
      draw =
          XDrawString(displayWindowContext.display, displayWindowContext.window,
                      displayWindowContext.gc, 10, 10, buffer, strlen(buffer));
    }
    if (draw != 0) {
      printf("Error printing to window.\n");
      XCloseDisplay(displayWindowContext.display);
      return 1;
    }
  }

  XCloseDisplay(displayWindowContext.display);
  return 0;
}

char *parseSpecialKeysyms(const char *keysym) {
  char *buffer = "PREPARSE";
  if (!strcmp(keysym, "space")) {
    buffer = " ";
  }
  if (!strcmp(keysym, "colon")) {
    buffer = ":";
  }
  if (!strcmp(keysym, "semicolon")) {
    buffer = ";";
  }
  if (!strcmp(keysym, "comma")) {
    buffer = ",";
  }
  if (!strcmp(keysym, "apostrophe")) {
    buffer = "'";
  }
  if (!strcmp(keysym, "period")) {
    buffer = ".";
  }
  if (!strcmp(keysym, "minus")) {
    buffer = "-";
  }
  if (!strcmp(keysym, "equal")) {
    buffer = "=";
  }
  if (!strcmp(keysym, "backslash")) {
    buffer = "\\";
  }
  if (!strcmp(keysym, "slash")) {
    buffer = "/";
  }
  if (!strcmp(keysym, "BackSpace")) {
    buffer = "";
    // TODO: Remove previous character
  }
  if (!strcmp(keysym, "Return")) {
    buffer = "\n";
    // TODO: Run Command
  }
  if (strcmp(buffer, "PREPARSE"))
    return buffer;
  return keysym;
}

void printCommandOutput(struct DisplayWindowContext displayWindowContext,
                        struct WindowProperties windowProperties,
                        char *command) {
  char *outputBuffer = runCommand(command);
  char lines[MAX_LINES][MAX_CHARS_PER_LINE] = {0};
  splitIntoArrayOfLines(outputBuffer, lines);
  drawString(displayWindowContext, windowProperties, lines);
}

char *runCommand(char *command) {
  char parsedCommand[1024] = "";
  if (!strstr(command, "/bin"))
    strcat(parsedCommand, "/bin/");

  strcat(parsedCommand, command);

  // Redirect stderr to stdout
  strcat(parsedCommand, " 2>&1");
  printf("%s\n", parsedCommand);

  FILE *commandStream = NULL;

  commandStream = popen(parsedCommand, "r");
  if (commandStream == NULL) {
    printf("Failed to open command stream\n");
    exit(1);
  }

  char path[1035] = "";

  char *outputBuffer = (char *) malloc(100);
  strcpy(outputBuffer, "");

  while (fgets(path, sizeof(path), commandStream) != NULL) {
    printf("%s", path);
    strcat(outputBuffer, path);
  }

  printf("Output Buffer of Command: %s\n", outputBuffer);
  if (pclose(commandStream) != 0) {
    printf("Failed to close command stream\n");
  }
  return outputBuffer;
}

void splitIntoArrayOfLines(char *outputBuffer,
                           char lines[][MAX_CHARS_PER_LINE]) {
  int linesIndex, outputIndex, charInLineIndex = 0;

  while (outputBuffer[outputIndex] != '\0') {
    if (outputBuffer[outputIndex] == '\n') {
      printf("%d: %s\n", linesIndex, lines[linesIndex]);
      lines[linesIndex][charInLineIndex] = '\0';
      linesIndex++;
      charInLineIndex = 0;
      outputIndex++;
      continue;
    }
    lines[linesIndex][charInLineIndex] = outputBuffer[outputIndex];
    charInLineIndex++;
    outputIndex++;
  }

  if (lines[linesIndex][charInLineIndex] != '\0') {
    lines[linesIndex][charInLineIndex] = '\0';
  }
}

void drawString(struct DisplayWindowContext displayWindowContext,
                struct WindowProperties properties,
                char lines[][MAX_CHARS_PER_LINE]) {
  int xOffset = 20;
  int yOffset = 12;

  XClearWindow(displayWindowContext.display, displayWindowContext.window);

  for (int i = 0; i < MAX_LINES; i++) {
    int draw =
        XDrawString(displayWindowContext.display, displayWindowContext.window,
                    displayWindowContext.gc, xOffset,
                    yOffset * (i + 1), lines[i], strlen(lines[i]));
  }
}


char *removeLastCharacter(char *text) {
  int stringEndIndex = strlen(text);

  if (stringEndIndex == 0) {
    return text;
  }

  char *newText = text;
  newText[stringEndIndex - 1] = '\0';
  return newText;
}

char *returnCommandOutput(FILE *output, char *outputBuffer) {
  char path[1035];

  strcpy(outputBuffer, "");

  while (fgets(path, sizeof(path), output) != NULL) {
    strcat(outputBuffer, path);
  }

  pclose(output);
  return outputBuffer;
}

int isEscape(XEvent event) {
  if (event.xkey.keycode == 0x09)
    return 1;
  return 0;
}
