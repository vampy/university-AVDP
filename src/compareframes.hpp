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
    void doWork();

signals:
    // compare image finished
    void onCompare(const QImage&);

public slots:
    void compareFrame(const QImage&);

private:
    QQueue<QImage>* m_queue_process; // used for processing in doWork
    QMutex m_mutex_process_queue;

    QImage m_current_frame;          // latest frame in the process queue
    QImage m_original_current_frame; // used internally to compare with the next frame
    quint32 m_current_frame_id = 0;  // latest frame id in the process queue

    bool m_debug = false;
};

#endif // COMPAREFRAMES_H
