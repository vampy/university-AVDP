#include "screenshot.hpp"

Screenshot::Screenshot(qint8 screen_id, qint16 screen_x, qint16 screen_y, qint16 screen_width, qint16 screen_height)
{
    setScreen(screen_id, screen_x, screen_y, screen_width, screen_height);
}

void Screenshot::take(int msec)
{
    QTimer::singleShot(msec, [this] { this->take(); });
}

void Screenshot::take()
{
    QTime debug_time;
    debug_time.start();
    static qint32 debug_counter = 0;

    m_image = m_screen->grabWindow(0, m_screen_x, m_screen_y, m_screen_width, m_screen_height).toImage();
    Q_ASSERT(!m_image.isNull());

    // round up so that it is a multiple of block_width
    auto new_size = QSize(util::roundUp(m_image.width(), constants::BLOCK_WIDTH),
                          util::roundUp(m_image.height(), constants::BLOCK_WIDTH));

    if (m_image.size() != new_size)
    {
        // black pixels will be inserted if the crop is bigger than the actual image, which
        // will happen if the width or height is not a multiple of 16
        m_image = m_image.copy(0, 0, new_size.width(), new_size.height());
    }
    emit onScreenshot();

    if (debug_counter % static_cast<qint32>(constants::DEFAULT_FPS) == 0)
    {
        qInfo() << "Screenshot::take" << debug_time.elapsed() << "ms";
    }
    debug_counter++;
}

QImage Screenshot::getImage() const { return m_image; }

void Screenshot::setScreen(qint8 screen_id, qint16 screen_x, qint16 screen_y, qint16 screen_width, qint16 screen_height)
{
    // https://stackoverflow.com/questions/29988952/how-can-i-take-a-print-screen-using-qt-c-with-various-monitors
    // set screen
    auto screens = getScreens();
    if (screen_id < 0)
    {
        m_screen = QGuiApplication::primaryScreen();
    }
    else
    {
        Q_ASSERT(screen_id < screens.size());
        m_screen = screens[screen_id];
    }

    Q_ASSERT(m_screen);

    // set coordinates
    m_screen_x      = screen_x < 0 ? m_screen->geometry().x() : screen_x;
    m_screen_y      = screen_y < 0 ? m_screen->geometry().y() : screen_y;
    m_screen_width  = screen_width == 0 ? m_screen->geometry().width() : screen_width;
    m_screen_height = screen_height == 0 ? m_screen->geometry().height() : screen_height;

    qInfo() << "Screen id = " << screen_id << " | " << m_screen_width << "x" << m_screen_height << "at" << m_screen_x
            << "x" << m_screen_y;
}

QList<QScreen*> Screenshot::getScreens() const { return QGuiApplication::screens(); }
