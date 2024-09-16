#include "../inc/window.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 20
#define MAX_CHARS_PER_LINE 256

char *parseSpecialKeysyms(const char *keysym);
FILE *runCommandAndReturnOutput(char *command);
char *removeLastCharacter(char *text);
int drawString(Display *display, Window window,
               struct WindowProperties properties, char *buffer);
int isEscape(XEvent event);

char lineBuffers[MAX_LINES][MAX_CHARS_PER_LINE];
int indexOfLine = 0;

int main(int argc, char *argv[]) {
  Display *display = getDisplay();
  if (display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }

  Window window = createWindow(display);

  XEvent event;

  char buffer[MAX_CHARS_PER_LINE] = "";

  struct WindowProperties properties = createDefaultWindowProperties();

  while (True) {
    XNextEvent(display, &event);

    if (event.type != KeyPress)
      continue;

    if (isEscape(event))
      break;

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
    if (!strcmp(parsedKeysym, "\n")) {
      FILE *output = runCommandAndReturnOutput(buffer);
      char path[1035];

      strcpy(buffer, "");

      while (fgets(path, sizeof(path), output) != NULL) {
        strcat(buffer, path);
      }

      pclose(output);
      // parsedKeysym = "\n"; // Maybe this could help finding \n?
      parsedKeysym = "";
    }

    strcat(buffer, parsedKeysym);
    // printf("Buffer: %s\n", buffer);
    XClearWindow(display, window);
    int draw = drawString(display, window, properties, buffer);
    // int draw = XDrawString(display, window,
    //                        DefaultGC(display, properties.screenNumber), 10,
    //                        10, buffer, strlen(buffer));

    if (draw != 0) {
      printf("Error printing to window.\n");
      XCloseDisplay(display);
      return 1;
    }
  }

  // // respects grab_window, GrabModeSync, GrabModeSync
  // XGrabKeyboard(display, window, 0, 0, 0, CurrentTime);
  // XUngrabKeyboard(display, CurrentTime);

  XCloseDisplay(display);
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
  if (!strcmp(keysym, "BackSpace")) {
    buffer = "";
    // Remove previous character
  }
  if (!strcmp(keysym, "Return")) {
    buffer = "\n";
    // Run Command
  }
  if (strcmp(buffer, "PREPARSE"))
    return buffer;
  return keysym;
}

FILE *runCommandAndReturnOutput(char *command) {
  printf("Running command...\n");

  char parsedCommand[1024] = "";
  if (!strstr(command, "/bin"))
    strcat(parsedCommand, "/bin/");

  strcat(parsedCommand, command);
  printf("%s\n", parsedCommand);

  FILE *fp = NULL;

  fp = popen(parsedCommand, "r");
  if (fp == NULL) {
    printf("Failed to run command\n");
    exit(1);
  }

  return fp;
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

int drawString(Display *display, Window window,
               struct WindowProperties properties, char *buffer) {
  const int xOffset = 10;
  const int yOffset = 10;
  int x = xOffset;
  int y = yOffset;
  char drawBuff[MAX_CHARS_PER_LINE];
  /*
   * Make an array with all the lines possible to write on screen
   * If there's a \n, put that text to next array
   * If over the limit of lines, loop around
   */
  int bufferIndex = 0;
  int drawBufferIndex = 0;
  while (buffer[bufferIndex] != '\0' && bufferIndex < strlen(buffer)) {
    strcpy(drawBuff, "");
    while (buffer[bufferIndex] != '\n' && buffer[bufferIndex] != '\0' &&
           bufferIndex < strlen(buffer)) {
      drawBuff[drawBufferIndex] = buffer[bufferIndex];
      printf("Draw Buffer: %c", drawBuff[bufferIndex]);
      bufferIndex++;
      drawBufferIndex++;
    }
    printf("\n");
    if (buffer[bufferIndex] == '\n') {
      drawBuff[drawBufferIndex] = '\0';
      if (indexOfLine > MAX_LINES - 1)
        indexOfLine = 0;
      strcpy(lineBuffers[indexOfLine], drawBuff);
      printf("Line Buffer: %s\n", lineBuffers[indexOfLine]);
      int draw = XDrawString(
          display, window, DefaultGC(display, properties.screenNumber), x, y,
          lineBuffers[indexOfLine], strlen(lineBuffers[indexOfLine]));
      bufferIndex++;
      indexOfLine++;
    }
  }

  x += xOffset;
  y += yOffset;

  return 0;
}

int isEscape(XEvent event) {
  if (event.xkey.keycode == 0x09)
    return 1;
  return 0;
}
