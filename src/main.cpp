#include <QApplication>
#include <QDesktopWidget>

#include "screenshot_gui.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    ScreenshotGUI screenshot;
//    QApplication::desktop()->grab()
    screenshot.show();
    return app.exec();
}
