#include "../inc/window.h"
#include <X11/Xlib.h>
#include <stdio.h>

struct WindowProperties createDefaultWindowProperties() {
  int originX = 100;
  int originY = 100;
  int width = 250;
  int height = 100;
  int borderSize = 5;
  int screenNumber = 0;
  struct WindowProperties defaultProperties = {
      originX, originY, width, height, borderSize, screenNumber};

  return defaultProperties;
}

Display *getDisplay() {
  return XOpenDisplay(NULL);
}

Window createWindow(Display *display) {
  // Null argument uses value from env variable DISPLAY
  if (display == NULL) {
    printf("Error opening connection to X11.");
    return 1;
  }

  struct WindowProperties properties = createDefaultWindowProperties();

  Window window = XCreateSimpleWindow(
      display, DefaultRootWindow(display), properties.originX,
      properties.originY, properties.width, properties.height,
      properties.borderSize, BlackPixel(display, properties.screenNumber),
      WhitePixel(display, properties.screenNumber));

  if (window == 0) {
    printf("Error creating window.");
    XCloseDisplay(display);
    return 1;
  }

  XMapWindow(display, window);
  XSelectInput(display, window, KeyPressMask);

  return window;
}
