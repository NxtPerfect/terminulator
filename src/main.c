#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>

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

  if (window == NULL) {
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

    printf("Key pressed: %x\n", event.xkey.keycode);
    int draw = XDrawString(display, window, DefaultGC(display, screenNumber), 50, 50, "Button pressed", 14);

    if (draw != 0) {
      printf("Error printing to window.");
      XCloseDisplay(display);
      return 1;
    }

    char buffer[20];
    int lookup = XLookupString(event, buffer, 20);

    if (lookup != 0) {
      printf("Error printing character to window.");
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
