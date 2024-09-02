#include "../inc/window.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *parseSpecialKeysyms(const char *keysym);
FILE *runCommandAndReturnOutput(char *command);
char *removeLastCharacter(char *text);

int main(int argc, char *argv[]) {
  Display *display = getDisplay();
  if (display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }

  Window window = createWindow(display);

  XEvent event;

  char buffer[256] = "";

  struct WindowProperties properties = createDefaultWindowProperties();

  while (True) {
    XNextEvent(display, &event);

    if (event.type != KeyPress)
      continue;

    // Exit program on escape
    if (event.xkey.keycode == 0x09)
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

    if (!strcmp(parsedKeysym, "\n")) {
      FILE *output = runCommandAndReturnOutput(buffer);
      char path[1035];

      while (fgets(path, sizeof(path), output) != NULL) {
        strcat(buffer, path);
      }

      // close
      pclose(output);
      // strcpy(buffer, "");
      parsedKeysym = "";
    }

    if (!strcmp(parsedKeysym, "")) {
      removeLastCharacter(buffer);
    }
    strcat(buffer, parsedKeysym);
    // printf("Buffer: %s\n", buffer);
    XClearWindow(display, window);
    int draw = XDrawString(display, window,
                           DefaultGC(display, properties.screenNumber), 50, 50,
                           buffer, strlen(buffer));

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
    printf("Failed to run command\n" );
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
