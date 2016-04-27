#include "videostreamer.hpp"

VideoStreamer::VideoStreamer(QObject* parent,
                             quint8 fps,
                             qint16 screen_width,
                             qint16 screen_height,
                             QString hostname,
                             quint16 port)

    : QObject(parent),
      m_tcp_socket(new QTcpSocket(this)),
      m_fps(fps),
      m_screen_width(screen_width),
      m_screen_height(screen_height),
      m_hostname(hostname),
      m_port(port)
{

    // TODO find out if we really need this
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }

        m_network_session = new QNetworkSession(config, this);
        connect(m_network_session, &QNetworkSession::opened, this, &VideoStreamer::sessionOpened);
        m_network_session->open();
    }
}

void VideoStreamer::setConnectionInfo(QString hostname, quint16 port)
{
    m_hostname = hostname;
    m_port     = port;
}

void VideoStreamer::setResolution(qint16 width, qint16 height)
{
    m_screen_width  = width;
    m_screen_height = height;
}

void VideoStreamer::setFps(quint8 fps) { m_fps = fps; }

void VideoStreamer::initConnection()
{
    m_tcp_socket->abort();
    m_tcp_socket->connectToHost(m_hostname, m_port);
    m_tcp_socket->setParent(0);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << (quint32)0;

    out << m_screen_width << m_screen_height << m_fps;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    m_tcp_socket->write(block);
    m_tcp_socket->waitForBytesWritten();
    qInfo() << "Connected to server";
}

void VideoStreamer::onSendFrame(QQueue<Imageblock*> queue_blocks)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << (quint32)0;
    out << queue_blocks.size();

    while (!queue_blocks.empty())
    {
        Imageblock* imageblock = queue_blocks.dequeue();
        out << imageblock->getPosition() << imageblock->getImage();
        delete imageblock;
    }

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    m_tcp_socket->write(block);
    m_tcp_socket->waitForBytesWritten();
}

void VideoStreamer::sessionOpened() {}
