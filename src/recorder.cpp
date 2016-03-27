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
    if (m_queue->empty())
        return;

    if (m_currentFrame.isNull())
    {
        m_currentFrame = m_queue->dequeue();
        m_auxCurrentFrame = m_currentFrame.copy(0,0,m_currentFrame.width(),m_currentFrame.height());
    }
    else
    {
        int counter = 0;
        QImage nextFrame = m_queue->dequeue();
        for (int y = 0; y < nextFrame.height(); y += Constants::block_width)
        {
            for (int x = 0; x < nextFrame.width(); x += Constants::block_width)
            {
                Imageblock currentImageBlock(m_currentFrameId, QPoint(x, y),
                    m_auxCurrentFrame.copy(x, y, Constants::block_width, Constants::block_width));
                Imageblock nextImageBlock(m_currentFrameId + 1, QPoint(x, y),
                    nextFrame.copy(x, y, Constants::block_width, Constants::block_width));
                if (nextImageBlock.isEqualTo(currentImageBlock))
                {
                    counter++;
//                    for (int j = 0; j < Constants::block_width; j++)
//                    {
//                        for (int i = 0; i < Constants::block_width; i++)
//                        {
//                            m_currentFrame.setPixel(x + i, y + j, qRgb(255, 0, 0));
//                        }
//                    }
                }else{
                    //copy the blocks that are not equal from nextFrame to m_currentFrame.
                    for (int j = 0; j < Constants::block_width; j++)
                    {
                        for (int i = 0; i < Constants::block_width; i++)
                        {
                            m_auxCurrentFrame.setPixel(x + i, y + j,nextFrame.pixel(x+i,y+j));
                            m_currentFrame.setPixel(x + i, y + j,nextFrame.pixel(x+i,y+j));
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
                     << (m_currentFrame.width() / Constants::block_width)
                    * (m_currentFrame.height() / Constants::block_width)
                     << "equal blocks in frame" << m_currentFrameId;


        }
    }
}
