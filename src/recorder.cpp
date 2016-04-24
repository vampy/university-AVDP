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
      m_screen_width(screen_width),
      m_screen_height(screen_height),
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

    //send frame
    connect(m_compare,&CompareFrames::sendFrame,m_video_streamer,&VideoStreamer::onSendFrame);

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

    this->initConnection(hostname,port);
}

void Recorder::stopRecording() const
{
    qInfo() << "StopRecording!";
    m_timer->stop();
    m_screenshot->statsDisplay();
}

void Recorder::onScreenshot(const QImage& image)
{
    static qint32 default_fps = static_cast<qint32>(constants::DEFAULT_FPS);
    auto elapsed_time         = m_time_screenshot.elapsed();
    auto current_time         = QDateTime::currentMSecsSinceEpoch();

    if (m_count_screenshots % default_fps == 0)
    {
        qInfo() << "Recorder: take screenshot = " << elapsed_time
                << ", difference = " << current_time - m_last_time_screenshot;
    }

    m_last_time_screenshot = current_time;
    m_count_screenshots++;
    emit compareFrame(image);
}

void Recorder::onTimerTimeout()
{
    m_time_screenshot.start();
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
    m_video_streamer->setConnectionInfo(hostname,port);
    m_video_streamer->setResolution(m_screen_width,m_screen_height);
    qDebug()<<"Recorder::initConnection"<<m_screen_width<<" "<< m_screen_height;
    m_video_streamer->setFps((quint8)m_fps);
    m_video_streamer->initConnection();

}

void Recorder::setDebug(bool debug) { emit setDebugCompare(debug); }
