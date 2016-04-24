#include "compareframes.hpp"

CompareFrames::CompareFrames(QObject* parent) : QObject(parent), m_queue_process(new QQueue<QImage>) {}

void CompareFrames::setDebug(bool debug)
{
    qInfo() << "Debug set to " << debug;
    m_debug = debug;

    // prevent corruption
    if (m_original_current_frame.isNull() && debug)
        m_original_current_frame = m_current_frame.copy();
}

void CompareFrames::doWork()
{
    static QTime debug_time;
    static int debug_counter;
    static qint32 default_fps = static_cast<qint32>(constants::DEFAULT_FPS);

    // maybe this is better?
    // https://stackoverflow.com/questions/8682223/synchronizing-objects-in-different-threads-in-qt
    m_mutex_process_queue.lock();
    bool is_empty = m_queue_process->empty();
    m_mutex_process_queue.unlock();
    if (is_empty)
        return;

    debug_time.start();
    debug_counter = 0;

    // nothing to compare current frame with, first image
    if (m_current_frame.isNull())
    {
        m_mutex_process_queue.lock();
        m_current_frame = m_queue_process->dequeue();
        m_mutex_process_queue.unlock();

        if (m_debug)
        {
            m_original_current_frame = m_current_frame.copy();
        }

        // Send all blocks
        for (int x = 0; x < m_current_frame.width(); x += constants::BLOCK_WIDTH)
        {
            for (int y = 0; y < m_current_frame.height(); y += constants::BLOCK_WIDTH)
            {
                QImage current_frame      = m_current_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH);
                Imageblock* current_block = new Imageblock(m_current_frame_id, QPoint(x, y), current_frame);
                m_mutex_blocks_queue.lock();
                m_queue_blocks.enqueue(current_block);
                m_mutex_blocks_queue.unlock();
            }
        }

        emit sendFrame(m_queue_blocks);
        m_queue_blocks.clear();
        emit onCompare(m_current_frame);

        return;
    }

    // get raw next frame
    m_mutex_process_queue.lock();
    auto next_frame = m_queue_process->dequeue();
    m_mutex_process_queue.unlock();
    for (int x = 0; x < next_frame.width(); x += constants::BLOCK_WIDTH)
    {
        for (int y = 0; y < next_frame.height(); y += constants::BLOCK_WIDTH)
        {
            QImage current_frame;
            if (m_debug)
                current_frame = m_original_current_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH);
            else
                current_frame = m_current_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH);

            Q_ASSERT(!current_frame.isNull());
            Imageblock current_block(m_current_frame_id, QPoint(x, y), current_frame);
            Imageblock* next_block
                = new Imageblock(m_current_frame_id + 1,
                                 QPoint(x, y),
                                 next_frame.copy(x, y, constants::BLOCK_WIDTH, constants::BLOCK_WIDTH));

            if (*next_block == current_block)
            {
                debug_counter++;

                // only set red pixels in debug mode
                if (!m_debug)
                    continue;
                delete next_block;
                util::copyBlockColor(m_current_frame, qRgb(255, 0, 0), x, y);
            }
            else
            {
                // copy the blocks that are not equal from next frame to the current frame
                m_mutex_blocks_queue.lock();
                m_queue_blocks.enqueue(next_block);
                m_mutex_blocks_queue.unlock();

                util::copyBlock(m_current_frame, next_frame, x, y);
                if (m_debug)
                {
                    util::copyBlock(m_original_current_frame, next_frame, x, y);
                }
            }
        }
    }
    m_current_frame_id++;

    emit onCompare(m_current_frame);
    qDebug() << "Sent frame nr" << m_current_frame_id;
    emit sendFrame(m_queue_blocks);
    m_queue_blocks.clear();
    if (m_current_frame_id % default_fps == 0)
    {
        qInfo() << "We have " << debug_counter << "/"
                << (m_current_frame.width() / constants::BLOCK_WIDTH)
                       * (m_current_frame.height() / constants::BLOCK_WIDTH)
                << "equal blocks in frame" << m_current_frame_id;
        qInfo() << "Recorder::compareFrames" << debug_time.elapsed() << "ms";
    }
}

void CompareFrames::compareFrame(const QImage& image)
{
    m_mutex_process_queue.lock();
    m_queue_process->enqueue(image);
    m_mutex_process_queue.unlock();
    doWork();
}
