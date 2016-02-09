// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------


#include <QApplication>
#include "window.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window window;
    window.show();
  return app.exec();

}

