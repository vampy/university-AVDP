#ifndef COMPAREFRAMES_H
#define COMPAREFRAMES_H

#include <QDebug>
#include <QMutex>
#include <QPainter>
#include <QQueue>
#include <QTime>
#include "imageblock.hpp"

class QObject;

class CompareFrames : public QObject
{
    Q_OBJECT
public:
    explicit CompareFrames(QObject* parent = nullptr);

    void setDebug(bool debug);
    // Will return a null image on empty queue
    QImage getCurrentFrame();

    void addToProcessQueue(QImage image);
    void doWork();

signals:
    void onCompare();

public slots:
    void compareFrame(const QImage&);

private:
    // m_queue_process images will added to m_queue_display after processing.
    QQueue<QImage>* m_queue_process; // used for processing in doWork
    QQueue<QImage>* m_queue_display; // used for displaying
    QMutex m_mutex_process_queue;
    QMutex m_mutex_display_queue;

    QImage m_current_frame;          // latest frame in the process queue
    QImage m_original_current_frame; // used internally to compare with the next frame
    quint32 m_current_frame_id = 0;  // latest frame id in the process queue

    bool m_debug = false;
};

#endif // COMPAREFRAMES_H
