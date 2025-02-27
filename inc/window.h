#include <X11/X.h>
#include <X11/Xlib.h>

struct WindowProperties {
  int originX;
  int originY;
  int width;
  int height;
  int borderSize;
  int screenNumber;
};

struct DisplayWindowContext {
  Display *display;
  Window window;
  GC gc;
};

struct WindowProperties createDefaultWindowProperties();
Display *getDisplay();
Window createWindow(Display *display);
