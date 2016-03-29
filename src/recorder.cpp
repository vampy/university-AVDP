#include "recorder.hpp"

Recorder::Recorder(QObject* parent, qreal fps)
    : QObject(parent), m_queue(new QQueue<QImage>), m_screenshot(new Screenshot), m_timer(new QTimer(this)),
      m_workerTimer(new QTimer(this)), m_fps(fps)
{
    int interval = 1000 / m_fps;
    m_timer->setInterval(interval);
    m_timer->setSingleShot(false);
    m_workerTimer->setInterval(interval);
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
    if (m_queue->empty())
        return;

    if (m_currentFrame.isNull())
    {
        m_currentFrame = m_queue->dequeue();
        m_auxCurrentFrame = m_currentFrame.copy(0, 0, m_currentFrame.width(), m_currentFrame.height());
    }
    else
    {
        int counter = 0;
        QImage nextFrame = m_queue->dequeue();
        for (int x = 0; x < nextFrame.width(); x += constants::BLOCK_WIDTH)
        {
            for (int y = 0; y < nextFrame.height(); y += constants::BLOCK_WIDTH)
            {

                Imageblock currentImageBlock(m_currentFrameId, QPoint(x, y),
                    m_auxCurrentFrame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH));
                Imageblock nextImageBlock(m_currentFrameId + 1, QPoint(x, y),
                    nextFrame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH));
                if (nextImageBlock.isEqualTo(currentImageBlock))
                {
                    counter++;
                    //                    for (int j = 0; j < constants::BLOCK_WIDTH; j++)
                    //                    {
                    //                        for (int i = 0; i < constants::BLOCK_WIDTH; i++)
                    //                        {
                    //                            m_currentFrame.setPixel(x + i, y + j, qRgb(255, 0, 0));
                    //                        }
                    //                    }
                }
                else
                {
                    // copy the blocks that are not equal from nextFrame to m_currentFrame.
                    for (int i = 0; i < constants::BLOCK_WIDTH; i++)
                    {
                        for (int j = 0; j < constants::BLOCK_WIDTH; j++)
                        {
                            m_auxCurrentFrame.setPixel(x + i, y + j, nextFrame.pixel(x + i, y + j));
                            m_currentFrame.setPixel(x + i, y + j, nextFrame.pixel(x + i, y + j));
                        }
                    }
                }
            }
        }
        m_currentFrameId++;
        emit onFrameReady();
        if (m_currentFrameId % 25 == 0)
        {
            qDebug() << "We have " << counter << "/"
                     << (m_currentFrame.width() / constants::BLOCK_WIDTH)
                    * (m_currentFrame.height() / constants::BLOCK_WIDTH)
                     << "equal blocks in frame" << m_currentFrameId;
        }
    }
}
