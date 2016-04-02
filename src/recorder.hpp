#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <QQueue>
#include "screenshot.hpp"
#include "imageblock.hpp"
#include "constants.hpp"

class QObject;
class QTime;
class QDebug;

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject* parent = nullptr,
        qreal fps = constants::DEFAULT_FPS,
        qint8 screen_id = constants::DEFAULT_SCREEN,
        qint16 screen_x = constants::DEFAULT_SCREEN_POS,
        qint16 screen_y = constants::DEFAULT_SCREEN_POS,
        qint16 screen_width = constants::DEFAULT_SCREEN_SIZE,
        qint16 screen_height = constants::DEFAULT_SCREEN_SIZE);
    QImage getCurrentFrame();

    void setDebug(bool debug);

signals:
    void onFrameReady();

public slots:
    void startRecording();
    void stopRecording();

private slots:
    void takeScreenshot();
    void compareFrames();

private:
    QQueue<QImage>* m_queue;
    Screenshot* m_screenshot;
    QTimer* m_timer;
    QTimer* m_workerTimer;

    QImage m_current_frame;          // used to display
    QImage m_original_current_frame; // used internally to compare with the next frame
    quint32 m_current_frame_id = 0;

    qreal m_fps;

    bool m_debug = false;
};

#endif // RECORDER_HPP
