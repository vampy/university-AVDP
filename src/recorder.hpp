#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QQueue>
#include <QDebug>
#include <QTime>

#include "screenshot.hpp"
#include "imageblock.hpp"
#include "constants.hpp"

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject* parent = 0, qreal fps = constants::DEFAULT_FPS);
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

#endif // RECORDER_H
