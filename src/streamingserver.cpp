#include "streamingserver.hpp"

StreamingServer::StreamingServer(QObject* parent) : QObject(parent)
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
        connect(m_network_session, &QNetworkSession::opened, this, &StreamingServer::sessionOpened);

        //                statusLabel->setText("Opening network session.");
        m_network_session->open();
    }
    else
    {
        sessionOpened();
    }

    connect(m_tcp_server, &QTcpServer::newConnection, this, &StreamingServer::establishConnection);
}

StreamingServer::~StreamingServer() { m_tcp_server->close(); }

void StreamingServer::establishConnection()
{
    m_tcp_socket = m_tcp_server->nextPendingConnection();
    connect(m_tcp_socket, &QAbstractSocket::disconnected, m_tcp_socket, &QObject::deleteLater);
    connect(m_tcp_socket, &QIODevice::readyRead, this, &StreamingServer::readData);
    qDebug() << "New Connection!";
}

void StreamingServer::sessionOpened()
{
    // Save the used configuration
    if (m_network_session)
    {
        QNetworkConfiguration config = m_network_session->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = m_network_session->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    m_tcp_server = new QTcpServer(this);
    if (!m_tcp_server->listen(QHostAddress(constants::DEFAULT_HOSTNAME), constants::DEFAULT_PORT))
    {
        qDebug() << "Streaming Server. " << QString("Unable to start the server: %1.").arg(m_tcp_server->errorString());
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
        {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    qDebug()
        << (QString("The server is running on IP: %1 and Port: %2").arg(ipAddress).arg(m_tcp_server->serverPort()));
}

void StreamingServer::readData()
{
    QDataStream in(m_tcp_socket);
    in.setVersion(QDataStream::Qt_5_4);

    if (!m_streaming_started)
    {
        if (m_block_size == 0)
        {
            if (m_tcp_socket->bytesAvailable() < (int)sizeof(quint32))
                return;
            in >> m_block_size;
        }

        if (m_tcp_socket->bytesAvailable() < m_block_size)
            return;

        in >> m_screen_width >> m_screen_height >> m_fps;
        qDebug() << "Received fps" << m_fps;
        qDebug() << "Received width" << m_screen_width;
        qDebug() << "Received height" << m_screen_height;
        m_previous_frame = new QImage((int)m_screen_width, (int)m_screen_height, QImage::Format_RGB32);
        m_previous_frame->fill(Qt::red);
        m_streaming_started = true;
        m_block_size        = 0;
    }
    else
    {
        if (m_block_size == 0)
        {
            if (m_tcp_socket->bytesAvailable() < (int)sizeof(quint32))
                return;
            in >> m_block_size;
        }

        if (m_tcp_socket->bytesAvailable() < m_block_size)
            return;

        int no_of_blocks;

        in >> no_of_blocks;
        QPoint position;
        QImage image;

        for (int i = 0; i < no_of_blocks; i++)
        {
            in >> position >> image;
            int x = (position.x());
            int y = (position.y());
            for (int i = 0; i < image.width(); i++)
            {
                for (int j = 0; j < image.height(); j++)
                {
                    m_previous_frame->setPixel(i + x, j + y, image.pixel(i, j));
                }
            }
        }
        m_previous_frame->save("./screenshots/frame" + QString::number(m_current_frame_id) + ".jpg");
        qDebug() << "Received frame nr" << m_current_frame_id;
        m_block_size = 0;
        m_current_frame_id++;
    }
}
