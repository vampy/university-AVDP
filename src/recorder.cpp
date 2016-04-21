#include "recorder.hpp"

Recorder::Recorder(QObject* parent,
                   qreal fps,
                   qint8 screen_id,
                   qint16 screen_x,
                   qint16 screen_y,
                   qint16 screen_width,
                   qint16 screen_height)
    : QObject(parent),
      m_screenshot(new Screenshot()),
      m_compare(new CompareFrames()),
      m_thread_screenshot(new QThread(this)),
      m_thread_compare(new QThread(this)),
      m_timer(new QTimer(this)),
      m_fps(fps)
{
    m_screenshot->setScreen(screen_id, screen_x, screen_y, screen_width, screen_height);

    m_timer->setInterval(1000 / m_fps);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &Recorder::onTimerTimeout);

    // Connect to the thread, https://stackoverflow.com/questions/11033971/qt-thread-with-movetothread
    m_screenshot->moveToThread(m_thread_screenshot);
    m_compare->moveToThread(m_thread_compare);
    connect(m_thread_compare, &QThread::finished, m_compare, &QObject::deleteLater);
    connect(m_thread_screenshot, &QThread::finished, m_screenshot, &QObject::deleteLater);

    // take screenshot signal, TODO direct call to thread?
    typedef void (Screenshot::*ScreenshotVoidTake)(void);
    connect(this, &Recorder::takeScreenshot, m_screenshot, static_cast<ScreenshotVoidTake>(&Screenshot::take));

    // receive screenshot
    connect(m_screenshot, &Screenshot::onScreenshot, this, &Recorder::onScreenshot);

    // receive compare
    connect(m_compare, &CompareFrames::onCompare, this, &Recorder::onCompare);

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

QImage Recorder::getCurrentFrame() { return m_compare->getCurrentFrame(); }

void Recorder::startRecording() const
{
    qInfo() << "StartRecording!";
    m_screenshot->statsReset();
    m_timer->start();
}

void Recorder::stopRecording() const
{
    qInfo() << "StopRecording!";
    m_timer->stop();
    m_screenshot->statsDisplay();
}

void Recorder::onScreenshot()
{
    static qint32 default_fps = static_cast<qint32>(constants::DEFAULT_FPS);

    qint32 current_time = m_time_screenshot.elapsed();
    if (m_count_screenshots % default_fps == 0)
    {
        qInfo() << "Take screenshot: " << current_time - m_last_time_screenshot;
    }
    m_last_time_screenshot = current_time;

    m_count_screenshots++;
    m_compare->addToProcessQueue(m_screenshot->getImage());
}

void Recorder::onTimerTimeout()
{
    m_time_screenshot.start();
    emit takeScreenshot();
}

void Recorder::onCompare() { emit onFrameReady(); }

void Recorder::setDebug(bool debug) { m_compare->setDebug(debug); }
