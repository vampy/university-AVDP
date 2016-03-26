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
    m_image = screen->grabWindow(0).toImage();
    QSize newSize = QSize(ceil(m_image.width() / (double)m_blockWidth) * m_blockWidth,
        ceil(m_image.height() / (double)m_blockWidth) * m_blockWidth);
    if (m_image.size() != newSize)
    {
        // copy image pixel by pixel
        // the new image is pixel padded at right and bottom if needed.
        QImage newImage = QImage(newSize, m_image.format());
        for (int j = 0; j < newSize.height(); j++)
        {
            for (int i = 0; i < newSize.width(); i++)
            {
                newImage.setPixel(i, j, m_image.pixel(i < m_image.width() ? i : m_image.width() - 1,
                                            j < m_image.height() ? j : m_image.height() - 1));
            }
        }
        m_image = newImage;
    }
    // TODO remove and use only QImage
    m_pixmap = QPixmap::fromImage(m_image);
    emit onScreenshot();
}

QPixmap Screenshot::getPixmap() const { return m_pixmap; }

void Screenshot::setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; }

QImage Screenshot::getImage() const { return m_image; }

void Screenshot::setImage(const QImage& image) { m_image = image; }
