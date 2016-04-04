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
    static QTime debug_time;
    debug_time.start();

    // Maybe this will fail in some enviroments? https://doc.qt.io/qt-5/qscreen.html#grabWindow
    m_image = m_screen->grabWindow(0, m_screen_x, m_screen_y, m_new_screen_width, m_new_screen_height).toImage();
    Q_ASSERT(!m_image.isNull());
    emit onScreenshot();

    m_stats_take_total += debug_time.elapsed();
    m_stats_take_nr++;
}

QImage Screenshot::getImage() const { return m_image; }

void Screenshot::setScreen(qint8 screen_id, qint16 screen_x, qint16 screen_y, qint16 screen_width, qint16 screen_height)
{
    // https://stackoverflow.com/questions/29988952/how-can-i-take-a-print-screen-using-qt-c-with-various-monitors
    // set screen
    auto screens = getScreens();
    if (screen_id < constants::DEFAULT_SCREEN)
    {
        m_screen = QGuiApplication::primaryScreen();
    }
    else
    {
        Q_ASSERT(screen_id < screens.size());
        m_screen = screens[screen_id];
    }

    Q_ASSERT(m_screen);

    // set original coordinates and sizes
    m_screen_x      = screen_x <= constants::DEFAULT_SCREEN_POS ? m_screen->geometry().x() : screen_x;
    m_screen_y      = screen_y <= constants::DEFAULT_SCREEN_POS ? m_screen->geometry().y() : screen_y;
    m_screen_width  = screen_width == constants::DEFAULT_SCREEN_SIZE ? m_screen->geometry().width() : screen_width;
    m_screen_height = screen_height == constants::DEFAULT_SCREEN_SIZE ? m_screen->geometry().height() : screen_height;

    // round up so that it is a multiple of block_width
    auto new_size = QSize(util::roundUp(m_screen_width, constants::BLOCK_WIDTH),
                          util::roundUp(m_screen_height, constants::BLOCK_WIDTH));
    m_new_screen_width  = new_size.width();
    m_new_screen_height = new_size.height();

    if (m_new_screen_width != m_screen_width || m_new_screen_height != m_screen_height)
    {
        m_is_multiple = false;
    }

    qInfo() << "Screen id = " << screen_id << " | " << m_screen_width << "x" << m_screen_height << "at" << m_screen_x
            << "x" << m_screen_y;
    qInfo() << "Actual screen size after rounding up to multiple of block" << constants::BLOCK_WIDTH << "is"
            << m_new_screen_width << "x" << m_new_screen_height;
}

const QList<QScreen*> Screenshot::getScreens() const { return QGuiApplication::screens(); }

void Screenshot::statsDisplay()
{
    if (m_stats_take_nr == 0)
        m_stats_take_nr = 1;
    qInfo() << "Screenshot::take AVG = " << m_stats_take_total / m_stats_take_nr << "ms";
}

void Screenshot::statsReset() { m_stats_take_total = m_stats_take_nr = 0; }
