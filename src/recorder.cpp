#include "recorder.hpp"

Recorder::Recorder(QObject* parent, int fps)
    : QObject(parent), m_fps(fps), m_queue(new QQueue<QImage>), m_screenshot(new Screenshot), m_timer(new QTimer(this)),
      m_workerTimer(new QTimer(this))
{
    m_timer->setInterval(1000 / m_fps);
    m_timer->setSingleShot(false);
    m_workerTimer->setInterval(1000 / fps);
    m_workerTimer->setSingleShot(false);

    connect(m_timer, &QTimer::timeout, this, &Recorder::takeScreenshot);
    connect(m_workerTimer, &QTimer::timeout, this, &Recorder::compareFrames);
}

QImage Recorder::getCurrentFrame() { return m_currentFrame; }

void Recorder::startRecording()
{
    qDebug() << "StartRecording!";
    m_timer->start();
    m_workerTimer->start();
}

void Recorder::stopRecording()
{
    qDebug() << "StopRecording!";
    m_timer->stop();
    m_workerTimer->stop();
}

void Recorder::takeScreenshot()
{
    m_screenshot->take();
    m_queue->enqueue(m_screenshot->getImage());
}

void Recorder::compareFrames()
{
    if (!m_queue->empty())
    {
        if (m_currentFrame.isNull())
        {
            m_currentFrame = m_queue->dequeue();
        }
        else
        {
            int counter = 0;
            QImage nextFrame = m_queue->dequeue();
            for (int y = 0; y < nextFrame.height(); y += m_blockWidth)
            {
                for (int x = 0; x < nextFrame.width(); x += m_blockWidth)
                {
                    Imageblock currentImageBlock(
                        m_currentFrameId, QPoint(x, y), m_currentFrame.copy(x, y, m_blockWidth, m_blockWidth));
                    Imageblock nextImageBlock(
                        m_currentFrameId + 1, QPoint(x, y), nextFrame.copy(x, y, m_blockWidth, m_blockWidth));
                    if (nextImageBlock.isEqualTo(currentImageBlock))
                    {
                        counter++;
                    }
                }
            }
            m_currentFrameId++;
            m_currentFrame = nextFrame;
            emit onFrameReady();
            if (m_currentFrameId % 25 == 0)
            {
                qDebug() << "We have " << counter << "/"
                         << (m_currentFrame.width() / m_blockWidth) * (m_currentFrame.height() / m_blockWidth)
                         << "equal blocks in frame" << m_currentFrameId;
            }
        }
    }
}
