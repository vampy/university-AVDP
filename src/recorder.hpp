#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QQueue>
#include <QDebug>
#include "screenshot.hpp"
#include "imageblock.hpp"


class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = 0,int fps=25);
    QImage getCurrentFrame();

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
    QImage m_currentFrame;
    int m_currentFrameId = 0;
    int m_blockWidth = 16;
    int m_fps;

};

#endif // RECORDER_H
