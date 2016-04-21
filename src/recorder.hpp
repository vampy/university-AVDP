#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <QQueue>
#include <QThread>
#include "compareframes.hpp"
#include "constants.hpp"
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

    void setDebug(bool debug);
    void startRecording() const;
    void stopRecording() const;

signals:
    void onFrameReady();
    void takeScreenshot();

private slots:
    void onScreenshot();
    void onTimerTimeout();
    void onCompare();

private:
    Screenshot* m_screenshot;
    CompareFrames* m_compare;

    QThread* m_thread_screenshot;
    QThread* m_thread_compare;

    // the timer to take screenshots
    QTimer* m_timer;
    qreal m_fps;

    QTime m_time_screenshot; // timer to measure screenshot time
    qint32 m_last_time_screenshot = 0;
    quint32 m_count_screenshots   = 0;
};

#endif // RECORDER_HPP
