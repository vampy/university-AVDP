#include "cli.hpp"

CLI::CLI(QObject* parent, QCommandLineParser& parser)
    : QObject(parent), m_screenshot(new Screenshot), m_timer(new QTimer(this))
{
    m_timer->setInterval(1000 / 25); // 25 fps
    m_timer->setSingleShot(false);

    connect(m_timer, &QTimer::timeout, this, &CLI::takeScreenshot);
}

void CLI::quit()
{
    delete m_screenshot;
    m_screenshot = nullptr;
    emit finished();
}

void CLI::run()
{
    qDebug("CLI::run");

    // remove the screenshots dir if it does not exist
    QDir dir(m_dir_screenshots);
    if (dir.exists())
    {
        Q_ASSERT(dir.removeRecursively());
    }
    Q_ASSERT(dir.cdUp()); // go up from the screenshots dir
    Q_ASSERT(dir.mkdir(m_dir_screenshots));

    m_timer->start();
    m_elapsed.start();
}

void CLI::takeScreenshot()
{
    m_screenshot->take();
    Q_ASSERT(
        m_screenshot->getImage().save(QString("%1/%2.jpg").arg(m_dir_screenshots).arg(m_counter, 5, 10, QChar('0'))));
    m_counter++;

    // after run, convert to video
    // ffmpeg -i "%05d.jpg" -c:v libx264 -r 25 -pix_fmt yuv420p out.mp4
    if (m_counter == 100)
    {
        qDebug() << "Elapsed time (seconds): " << m_elapsed.elapsed() / 1000.0;
        m_timer->stop();
//        qDebug() <<"ScreenShotSize:"<< m_screenshot->getImage().size();
        quit();

    }
}
