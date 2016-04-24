#include "recorder.hpp"

Recorder::Recorder(QObject* parent,
                   qreal fps,
                   qint8 screen_id,
                   qint16 screen_x,
                   qint16 screen_y,
                   qint16 screen_width,
                   qint16 screen_height)
    : QObject(parent),
      m_queue_display(new QQueue<QImage>),
      m_screenshot(new Screenshot),
      m_compare(new CompareFrames),
      m_video_streamer(new VideoStreamer),
      m_thread_screenshot(new QThread(this)),
      m_thread_compare(new QThread(this)),
      m_timer(new QTimer(this)),
      m_fps(fps),
      m_screen_width(screen_width),
      m_screen_height(screen_height)
{
    m_screenshot->setScreen(screen_id, screen_x, screen_y, screen_width, screen_height);

    setTimer();
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &Recorder::onTimerTimeout);

    // Connect to the thread, https://stackoverflow.com/questions/11033971/qt-thread-with-movetothread
    m_screenshot->moveToThread(m_thread_screenshot);
    m_compare->moveToThread(m_thread_compare);

    connect(m_thread_compare, &QThread::finished, m_compare, &QObject::deleteLater);
    connect(m_thread_screenshot, &QThread::finished, m_screenshot, &QObject::deleteLater);

    // take screenshot
    typedef void (Screenshot::*ScreenshotVoidTake)(void);
    connect(this, &Recorder::takeScreenshot, m_screenshot, static_cast<ScreenshotVoidTake>(&Screenshot::take));

    // receive screenshot
    connect(m_screenshot, &Screenshot::onScreenshot, this, &Recorder::onScreenshot);

    // compare frame
    connect(this, &Recorder::compareFrame, m_compare, &CompareFrames::compareFrame);
    connect(this, &Recorder::setDebugCompare, m_compare, &CompareFrames::setDebug);

    // receive compare
    connect(m_compare, &CompareFrames::onCompare, this, &Recorder::onCompare);

    // send frame
    connect(m_compare, &CompareFrames::sendFrame, m_video_streamer, &VideoStreamer::onSendFrame);

    qInfo() << "Current FPS =" << m_fps;
    m_thread_screenshot->start();
    m_thread_compare->start();
}

Recorder::~Recorder()
{
    m_thread_screenshot->quit();
    m_thread_compare->quit();
    m_thread_screenshot->wait();
    m_thread_compare->wait();
}

QImage Recorder::getCurrentFrame()
{
    m_mutex_display_queue.lock();
    QImage current;
    if (!m_queue_display->isEmpty())
        current = m_queue_display->dequeue();
    m_mutex_display_queue.unlock();
    return current;
}

void Recorder::startRecording(QString hostname, quint16 port)
{
    qInfo() << "StartRecording!";
    m_screenshot->statsReset();
    m_timer->start();

    initConnection(hostname, port);

    m_throttle = false;
}

void Recorder::stopRecording()
{
    qInfo() << "StopRecording!";
    m_timer->stop();
    m_screenshot->statsDisplay();
    m_throttle = false;
}

void Recorder::onScreenshot(const QImage& image)
{
    static qint32 default_fps    = static_cast<qint32>(constants::DEFAULT_FPS);
    auto elapsed_take_screenshot = m_time_take_screenshot.elapsed();
    auto elapsed_on_screenshot   = m_time_on_screenshot.elapsed();

    if (m_count_screenshots % default_fps == 0)
    {
        qInfo() << "Recorder: take screenshot = " << elapsed_take_screenshot
                << ", difference = " << elapsed_on_screenshot;
    }

    // TODO the opposite of this until the original fps?
    if (!m_throttle && elapsed_on_screenshot - m_max_throttle > 10)
    {
        m_throttle = true;
        m_fps--;
        qInfo() << "\nToo slow :(. Throttling FPS to " << m_fps << "\n";
    }

    m_count_screenshots++;
    m_time_on_screenshot.start();
    emit compareFrame(image);
}

void Recorder::onTimerTimeout()
{
    if (m_throttle)
    {
        setTimer();
        m_throttle = false;
    }

    m_time_take_screenshot.start();
    emit takeScreenshot();
}

void Recorder::onCompare(const QImage& image)
{
    m_mutex_display_queue.lock();
    m_queue_display->enqueue(image);
    m_mutex_display_queue.unlock();
    emit onFrameReady();
}

void Recorder::initConnection(QString hostname, quint16 port)
{
    m_video_streamer->setConnectionInfo(hostname, port);
    m_video_streamer->setResolution(m_screen_width, m_screen_height);
    qDebug() << "Recorder::initConnection" << m_screen_width << " " << m_screen_height;
    m_video_streamer->setFps((quint8)m_fps);
    m_video_streamer->initConnection();
}

void Recorder::setTimer()
{
    m_max_throttle = 1000 / m_fps;
    m_timer->setInterval(m_max_throttle);
}

void Recorder::setDebug(bool debug) { emit setDebugCompare(debug); }
