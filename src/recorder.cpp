#include "recorder.hpp"

Recorder::Recorder(QObject* parent,
                   qreal fps,
                   qint8 screen_id,
                   qint16 screen_x,
                   qint16 screen_y,
                   qint16 screen_width,
                   qint16 screen_height)
    : QObject(parent),
      m_queue(new QQueue<QImage>),
      m_screenshot(new Screenshot(screen_id, screen_x, screen_y, screen_width, screen_height)),
      m_timer(new QTimer(this)),
      m_workerTimer(new QTimer(this)),
      m_fps(fps)
{
    int interval = 1000 / m_fps;
    m_timer->setInterval(interval);
    m_timer->setSingleShot(false);
    m_workerTimer->setInterval(interval);
    m_workerTimer->setSingleShot(false);

    connect(m_timer, &QTimer::timeout, this, &Recorder::onTimerTimeout);
    connect(m_workerTimer, &QTimer::timeout, this, &Recorder::compareFrames);

    // Move to thread
    m_screenshot->moveToThread(&m_thread_screenshot);
    connect(&m_thread_screenshot, &QThread::finished, m_screenshot, &QObject::deleteLater);

    // take screenshot signal
    typedef void (Screenshot::*ScreenshotVoidTake)(void);
    connect(this, &Recorder::takeScreenshot, m_screenshot, static_cast<ScreenshotVoidTake>(&Screenshot::take));

    // receive screenshot
    connect(m_screenshot, &Screenshot::onScreenshot, this, &Recorder::onScreenshot);

    // start
    m_thread_screenshot.start();
}

Recorder::~Recorder()
{
    m_thread_screenshot.quit();
    m_thread_screenshot.wait();
}

QImage Recorder::getCurrentFrame() { return m_current_frame; }

void Recorder::startRecording()
{
    qInfo() << "StartRecording!";
    m_timer->start();
    m_workerTimer->start();
}

void Recorder::stopRecording()
{
    qInfo() << "StopRecording!";
    m_timer->stop();
    m_workerTimer->stop();
}

void Recorder::onScreenshot() { m_queue->enqueue(m_screenshot->getImage()); }

void Recorder::onTimerTimeout() { emit takeScreenshot(); }

void Recorder::compareFrames()
{
    QTime debug_time;
    debug_time.start();
    int debug_counter = 0;

    if (m_queue->empty())
        return;

    // nothing to compare current frame with
    if (m_current_frame.isNull())
    {
        m_current_frame = m_queue->dequeue();
        if (m_debug)
        {
            m_original_current_frame = m_current_frame.copy();
        }

        return;
    }

    // get raw next frame
    auto next_frame = m_queue->dequeue();
    for (int x = 0; x < next_frame.width(); x += constants::BLOCK_WIDTH)
    {
        for (int y = 0; y < next_frame.height(); y += constants::BLOCK_WIDTH)
        {
            QImage current_frame;
            if (m_debug)
                current_frame = m_original_current_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH);
            else
                current_frame = m_current_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH);

            Q_ASSERT(!current_frame.isNull());
            Imageblock current_block(m_current_frame_id, QPoint(x, y), current_frame);
            Imageblock next_block(m_current_frame_id + 1,
                                  QPoint(x, y),
                                  next_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH));

            if (next_block == current_block)
            {
                debug_counter++;

                // only set red pixels in debug mode
                if (!m_debug)
                    continue;

                for (int i = 0; i < constants::BLOCK_WIDTH; i++)
                {
                    for (int j = 0; j < constants::BLOCK_WIDTH; j++)
                    {
                        m_current_frame.setPixel(x + i, y + j, qRgb(255, 0, 0));
                    }
                }
            }
            else
            {
                // copy the blocks that are not equal from next frame to the current frame
                for (int i = 0; i < constants::BLOCK_WIDTH; i++)
                {
                    for (int j = 0; j < constants::BLOCK_WIDTH; j++)
                    {
                        if (m_debug)
                        {
                            m_original_current_frame.setPixel(x + i, y + j, next_frame.pixel(x + i, y + j));
                        }

                        m_current_frame.setPixel(x + i, y + j, next_frame.pixel(x + i, y + j));
                    }
                }
            }
        }
    }
    m_current_frame_id++;
    emit onFrameReady();

    if (m_current_frame_id % static_cast<qint32>(constants::DEFAULT_FPS) == 0)
    {
        qInfo() << "We have " << debug_counter << "/"
                << (m_current_frame.width() / constants::BLOCK_WIDTH)
                       * (m_current_frame.height() / constants::BLOCK_WIDTH)
                << "equal blocks in frame" << m_current_frame_id;
        qInfo() << "Recorder::compareFrames" << debug_time.elapsed() << "ms";
    }
}

void Recorder::setDebug(bool debug)
{
    m_debug = debug;

    // prevent coruption
    if (m_original_current_frame.isNull() && debug)
        m_original_current_frame = m_current_frame.copy();
}
