#include "videostreamer.h"

VideoStreamer::VideoStreamer(QObject* parent,
                             quint8 fps,
                             qint16 screen_width,
                             qint16 screen_height,
                             QString hostname,
                             quint16 port)

    : QObject(parent),
      m_fps(fps),
      m_screen_width(screen_width),
      m_screen_height(screen_height),
      m_hostname(hostname),
      m_port(port),
      m_tcp_socket(new QTcpSocket(this)),
      m_network_session(nullptr)
{

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

void VideoStreamer::initConnection()
{
    m_tcp_socket->abort();
    m_tcp_socket->connectToHost(m_hostname, m_port);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_4_0);

    out << (quint8)0;

    out << m_screen_width << m_screen_height << m_fps;

    out.device()->seek(0);
    out << (quint8)(block.size() - sizeof(quint8));

    m_tcp_socket->write(block);
}

void VideoStreamer::sessionOpened() {}
