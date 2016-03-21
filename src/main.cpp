#include <QApplication>
#include <QDesktopWidget>

#include "screenshot.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Screenshot screenshot;
    screenshot.move(QApplication::desktop()->availableGeometry(&screenshot).topLeft() + QPoint(20, 20));
    screenshot.show();
    return app.exec();
}
