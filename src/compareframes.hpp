#ifndef COMPAREFRAMES_H
#define COMPAREFRAMES_H

#include <QDebug>
#include <QLinkedList>
#include <QMutex>
#include <QPainter>
#include <QTime>
#include <QVector>
#include "imageblock.hpp"

class QObject;

class CompareFrames : public QObject
{
    Q_OBJECT
public:
    explicit CompareFrames(QObject* parent = nullptr);
    void doWork();

signals:
    // compare image finished
    void onCompare(const QImage&);
    void sendFrame(const QVector<Imageblock*>);

public slots:
    void compareFrame(const QImage&);
    void setDebug(bool);

private:
    QLinkedList<QImage>* m_queue_process; // used for processing in doWork
    QVector<Imageblock*> m_vector_blocks; // used for storing all the image blocks different from the previous frame
    QMutex m_mutex_queue_process;
    QMutex m_mutex_vector_blocks;

    QImage m_current_frame;          // latest frame in the process queue
    QImage m_original_current_frame; // used internally to compare with the next frame
    quint32 m_current_frame_id = 0;  // latest frame id in the process queue

    bool m_debug = false;
};

#endif // COMPAREFRAMES_H
