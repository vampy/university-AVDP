#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <QQueue>
#include <QThread>
#include "compareframes.hpp"
#include "imageblock.hpp"
#include "screenshot.hpp"
#include "videostreamer.h"

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
    void startRecording(QString hostname, quint16 port);
    void stopRecording();

    void setDebug(bool);

private slots:
    void onScreenshot(const QImage&);
    void onTimerTimeout();
    void onCompare(const QImage&);

private:
    void initConnection(QString hostname, quint16 port);

    // Sets the fps, max_thorrtle and rthe timer interval
    void setTimer();

    QQueue<QImage>* m_queue_display; // used for displaying
    QMutex m_mutex_display_queue;

    Screenshot* m_screenshot;
    CompareFrames* m_compare;
    VideoStreamer* m_video_streamer;

    QThread* m_thread_screenshot;
    QThread* m_thread_compare;

    // the timer to take screenshots
    QTimer* m_timer;
    qreal m_fps;
    qint16 m_screen_width;
    qint16 m_screen_height;

    QTime m_time_take_screenshot; // timer to measure take screenshot time
    QTime m_time_on_screenshot;
    quint32 m_count_screenshots = 0;

    // reduce the number of screenshots if the machine can't take it aka reduce fps
    quint8 m_max_throttle;
    bool m_throttle = false;
};

#endif // RECORDER_HPP
