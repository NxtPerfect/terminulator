#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

char *parseSpecialCharacters(const char *keysym);

int main(int argc, char *argv[])
{
  XEvent event;
  // Null argument uses value from env variable DISPLAY
  Display* display = XOpenDisplay(NULL);
  if (display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }
  
  // Placeholder values
  int originX = 100;
  int originY = 100;
  int width = 250;
  int height = 100;
  int borderSize = 5;
  int screenNumber = 0;
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), originX, originY, width, height, borderSize, BlackPixel(display, screenNumber), WhitePixel(display, screenNumber));

  if (window == 0) {
    printf("Error creating window.");
    XCloseDisplay(display);
    return 1;
  }

  XMapWindow(display, window);
  XSelectInput(display, window, KeyPressMask);

  while (True) {
    XNextEvent(display, &event);
    
    if (event.type != KeyPress) continue;

    // Exit program on escape
    if (event.xkey.keycode == 0x09) break;

    KeySym keysym = XLookupKeysym(&event.xkey, 0);

    if (keysym != 0) {
      printf("Error looking up keysym.");
    }

    const char *keysymString = XKeysymToString(keysym);

    if (keysymString == NULL) {
      printf("Failed changing keysym to string.");
    }

    char buffer[256];
    printf("Key pressed here: %x\n", event.xkey.keycode);
    printf("Key pressed here: %s\n", keysymString);

    const char *parsedString = parseSpecialCharacters(keysymString);
    // strcat(buffer, keysymString);
    strcat(buffer, parsedString);
    XClearWindow(display, window);
    int draw = XDrawString(display, window, DefaultGC(display, screenNumber), 50, 50, buffer, strlen(buffer));

    if (draw != 0) {
      printf("Error printing to window.");
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

char *parseSpecialCharacters(const char *keysym) {
  char *buffer = "";
  if (strcmp(keysym, "space")) {
    buffer = " ";
  }
  if (strcmp(keysym, "colon")) {
    buffer = ":";
  }
  if (strcmp(keysym, "semicolon")) {
    buffer = ";";
  }
  if (strcmp(keysym, "comma")) {
    buffer = ",";
  }
  if (strcmp(keysym, "period")) {
    buffer = ".";
  }
  if (strcmp(keysym, "minus")) {
    buffer = "-";
  }
  if (strcmp(keysym, "equal")) {
    buffer = "=";
  }
  if (strcmp(keysym, "backspace")) {
    buffer = "";
    // Remove previous character
  }
  if (strcmp(keysym, "enter")) {
    buffer = "\n";
    // Run Command
  }
  if (buffer != NULL) return buffer;
  return keysym;
}
