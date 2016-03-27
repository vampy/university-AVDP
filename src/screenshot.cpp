#include "screenshot.hpp"
#include <QTime>
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
    // TODO: select screen from cli and GUI
    // https://stackoverflow.com/questions/29988952/how-can-i-take-a-print-screen-using-qt-c-with-various-monitors
    QScreen* screen;
    auto screens = QGuiApplication::screens();

    if (screens.size() > 1)
    {
        // grab first screen
        screen = screens[0];
    }
    else
    {
        screen = QGuiApplication::primaryScreen();
    }

    if (!screen)
    {
        qDebug() << "Screenshot::shootScreen could not get screen";
        return;
    }

    m_image = screen->grabWindow(0, screen->geometry().x(), screen->geometry().y(), screen->geometry().width(),
                          screen->geometry().height()).toImage();
    Q_ASSERT(!m_image.isNull());

    // round up so that it is a multiple of block_width
    auto new_size = QSize(util::roundUp(m_image.width(), Constants::block_width),
        util::roundUp(m_image.height(), Constants::block_width));

    if (m_image.size() != new_size)
    {
        QTime t;
        t.start();

        // black pixels will be inserted if the crop is bigger than the actual image, which
        // will happen if the width or height is not a multiple of 16
        m_image = m_image.copy(0, 0, new_size.width(), new_size.height());

        //        qDebug() << "Screenshot: copy image" << t.elapsed() << "ms";
    }
    // TODO remove and use only QImage
    m_pixmap = QPixmap::fromImage(m_image);
    Q_ASSERT(!m_pixmap.isNull());
    emit onScreenshot();
}

QPixmap Screenshot::getPixmap() const { return m_pixmap; }

void Screenshot::setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; }

QImage Screenshot::getImage() const { return m_image; }

void Screenshot::setImage(const QImage& image) { m_image = image; }
