#include "screenshot.hpp"

Screenshot::Screenshot() {}

void Screenshot::take(int msec)
{
    QTimer::singleShot(msec, [this]
        {
            this->take();
        });
}

void Screenshot::take()
{
    // TODO: select screen, see
    // https://stackoverflow.com/questions/29988952/how-can-i-take-a-print-screen-using-qt-c-with-various-monitors
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen)
    {
        qDebug() << "Screenshot::shootScreen could not get screen";
        return;
    }
    m_pixmap = screen->grabWindow(0);

    emit onScreenshot();
}

QPixmap Screenshot::getPixmap() const { return m_pixmap; }

void Screenshot::setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; }
