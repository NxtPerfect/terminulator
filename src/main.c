#include "../inc/main.h"
#include <X11/X.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char lineBuffers[MAX_LINES][MAX_CHARS_PER_LINE];
int indexOfLine = 0;

int main(int argc, char *argv[]) {
  struct DisplayWindowContext displayWindowContext;
  displayWindowContext.display = getDisplay();
  if (displayWindowContext.display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }

  struct WindowProperties properties = createDefaultWindowProperties();

  displayWindowContext.window = createWindow(displayWindowContext.display);
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

    const char *keysymString = getKeysymToString(&event.xkey);

    if (keysymString == NULL) {
      printf("Failed changing keysym to string.\n");
    }

    printf("Key pressed here: %s\n", keysymString);

    const char *parsedKeysym = parseSpecialKeysyms(keysymString);

    // BackSpace
    if (!strcmp(parsedKeysym, "")) {
      removeLastCharacter(buffer);
      drawString(displayWindowContext, buffer);
      continue;
    }

    // Enter
    if (!strcmp(parsedKeysym, "\n")) {
      printCommandOutput(displayWindowContext, properties, buffer);
      strcpy(buffer, "");
      continue;
    }

    strcat(buffer, parsedKeysym);
    XClearWindow(displayWindowContext.display, displayWindowContext.window);
    int draw =
        XDrawString(displayWindowContext.display, displayWindowContext.window,
                    displayWindowContext.gc, 10, 10, buffer, strlen(buffer));

    if (draw != 0) {
      printf("Error printing to window.\n");
      XCloseDisplay(displayWindowContext.display);
      return 1;
    }
  }

  XCloseDisplay(displayWindowContext.display);
  return 0;
}

char *getKeysymToString(XKeyEvent *xkey) {
  KeySym keysym = XLookupKeysym(xkey, 0);

  if (keysym == 0) {
    printf("Error looking up keysym %lu \n", keysym);
  }

  return XKeysymToString(keysym);
}

const char *parseSpecialKeysyms(const char *keysym) {
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
  }
  if (!strcmp(keysym, "Return")) {
    buffer = "\n";
  }
  if (!strcmp(keysym, "Shift_L") || !strcmp(keysym, "Shift_R")) {
    buffer = "^";
  }
  if (!strcmp(buffer, "PREPARSE"))
    return keysym;
  return buffer;
}

void printCommandOutput(struct DisplayWindowContext displayWindowContext,
                        struct WindowProperties windowProperties,
                        char *command) {
  char *outputBuffer = runCommand(command);
  char lines[MAX_LINES][MAX_CHARS_PER_LINE] = {0};
  splitIntoArrayOfLines(outputBuffer, lines);
  drawLines(displayWindowContext, windowProperties, lines);
  free(outputBuffer);
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
  printf("Created path\n");

  char *outputBuffer = (char *)malloc((100 + 1) * sizeof(char));
  printf("Created output buffer\n");

  if (outputBuffer == NULL) {
    printf("Failed to create output buffer\n");
    exit(1);
  }

  strcpy(outputBuffer, "");
  printf("Cleared output buffer\n");

  int pathSize = sizeof(path);
  while (fgets(path, pathSize, commandStream) != NULL) {
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

      if (isSafeIncrementLines(linesIndex)) {
        linesIndex++;
      } else {
        linesIndex = 0;
      }
      charInLineIndex = 0;
      outputIndex++;
      continue;
    }

    lines[linesIndex][charInLineIndex] = outputBuffer[outputIndex];
    if (isSafeIncrementChars(charInLineIndex))
      charInLineIndex++;
    else if (isSafeIncrementLines(linesIndex)) {
      linesIndex++;
      charInLineIndex = 0;
    } else {
      linesIndex = 0;
      charInLineIndex = 0;
    }
    outputIndex++;
  }

  if (lines[linesIndex][charInLineIndex] != '\0') {
    printf("End of output, adding end to string");
    lines[linesIndex][charInLineIndex] = '\0';
  }
}

int isSafeIncrementLines(int linesIndex) {
  if (linesIndex >= MAX_LINES)
    return -1;
  return 1;
}

int isSafeIncrementChars(int charInLineIndex) {
  if (charInLineIndex >= MAX_CHARS_PER_LINE)
    return -1;
  return 1;
}

void drawLines(struct DisplayWindowContext displayWindowContext,
               struct WindowProperties properties,
               char lines[][MAX_CHARS_PER_LINE]) {
  XClearWindow(displayWindowContext.display, displayWindowContext.window);

  for (int i = 0; i < MAX_LINES; i++) {
    int draw =
        XDrawString(displayWindowContext.display, displayWindowContext.window,
                    displayWindowContext.gc, X_LINES_OFFSET,
                    Y_LINES_OFFSET * (i + 1), lines[i], strlen(lines[i]));
  }
}

void drawString(struct DisplayWindowContext displayWindowContext,
                char *buffer) {
  XClearWindow(displayWindowContext.display, displayWindowContext.window);
  int draw =
      XDrawString(displayWindowContext.display, displayWindowContext.window,
                  displayWindowContext.gc, 10, 10, buffer, strlen(buffer));
  if (draw != 0) {
    printf("Error printing to window.\n");
    XCloseDisplay(displayWindowContext.display);
    return;
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
