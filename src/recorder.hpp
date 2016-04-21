#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <QQueue>
#include <QThread>
#include "compareframes.hpp"
#include "imageblock.hpp"
#include "screenshot.hpp"

class QObject;
class QTime;
class QDebug;

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject* parent      = nullptr,
                      qreal fps            = constants::DEFAULT_FPS,
                      qint8 screen_id      = constants::DEFAULT_SCREEN,
                      qint16 screen_x      = constants::DEFAULT_SCREEN_POS,
                      qint16 screen_y      = constants::DEFAULT_SCREEN_POS,
                      qint16 screen_width  = constants::DEFAULT_SCREEN_SIZE,
                      qint16 screen_height = constants::DEFAULT_SCREEN_SIZE);
    ~Recorder();
    QImage getCurrentFrame();

signals:
    // used by GUI
    void onFrameReady();

    // used by screenshot
    void takeScreenshot();

    // used by compare frames
    void compareFrame(const QImage&);

    void setDebugCompare(bool);

public slots:
    void startRecording() const;
    void stopRecording() const;
    void setDebug(bool);

private slots:
    void onScreenshot(const QImage&);
    void onTimerTimeout();
    void onCompare(const QImage&);

private:
    QQueue<QImage>* m_queue_display; // used for displaying
    QMutex m_mutex_display_queue;

    Screenshot* m_screenshot;
    CompareFrames* m_compare;

    QThread* m_thread_screenshot;
    QThread* m_thread_compare;

    // the timer to take screenshots
    QTimer* m_timer;
    qreal m_fps;

    QTime m_time_screenshot; // timer to measure screenshot time
    qint64 m_last_time_screenshot = 0;
    quint32 m_count_screenshots   = 0;
};

#endif // RECORDER_HPP
