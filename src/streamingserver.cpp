#include "streamingserver.hpp"

StreamingServer::StreamingServer(QObject* parent) : QObject(parent)
{
    //    m_timer_read->setSingleShot(true);
    //    connect(m_timer_read, &QTimer::timeout, this, &StreamingServer::readyRead);

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
    if (!util::createDir(m_dir_save))
    {
        qInfo() << "Can't create dir for screenshots :(";
    }

    m_tcp_socket    = m_tcp_server->nextPendingConnection();
    m_in_datastream = new QDataStream(m_tcp_socket);
    m_in_datastream->setVersion(QDataStream::Qt_5_4);

    connect(m_tcp_socket, &QAbstractSocket::disconnected, this, &StreamingServer::disconnected);
    connect(m_tcp_socket, &QIODevice::readyRead, this, &StreamingServer::readyRead);
    qInfo() << "New Connection!";
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
        qInfo() << "Streaming Server. " << QString("Unable to start the server: %1.").arg(m_tcp_server->errorString());
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

    qInfo() << QString("The server is running on IP: %1 and Port: %2").arg(ipAddress).arg(m_tcp_server->serverPort());
}

// See http://doc.qt.io/qt-5/datastreamformat.html
void StreamingServer::readyRead()
{
    auto bytesAvailable = m_tcp_socket->bytesAvailable();

    // No data to read
    if (!bytesAvailable)
        return;

    // Set initial block size
    if (m_block_size == 0)
    {
        if (bytesAvailable < static_cast<qint64>(sizeof(quint32)))
        {
            qInfo() << "WARNING: Can't get enough bytes to determine the block size. bytesAvailable ="
                    << bytesAvailable;
            return;
        }

        *m_in_datastream >> m_block_size;
        if (constants::DEBUG_NETWORK)
            qInfo() << "Received data block size (bytes) = " << m_block_size;
    }

    // The packet is too large, TCP will split it.
    // readyRead signal will be called every time a new packet is received.
    // Do not process the current frame until we don't have the bytes available in the OS TCP queue.
    if (bytesAvailable < m_block_size)
    {
        qInfo() << "WARNING: Can't get enough bytes to match the m_block_size =" << m_block_size
                << " ||| bytesAvailable =" << bytesAvailable;
        return;
    }

    if (constants::DEBUG_NETWORK)
    {
        qInfo() << "bytesAvailable = " << bytesAvailable;
    }

    // initial connection
    if (!m_streaming_started)
    {
        *m_in_datastream >> m_screen_width >> m_screen_height >> m_fps;
        qInfo() << "Received fps =" << m_fps;
        qInfo() << "Received width =" << m_screen_width;
        qInfo() << "Received height =" << m_screen_height;
        m_previous_frame = new QImage((int)m_screen_width, (int)m_screen_height, QImage::Format_RGB32);
        m_previous_frame->fill(Qt::red);

        if (m_screen_width <= 0 || m_screen_height <= 0 || !m_fps)
        {
            qCritical() << "You tripping? How can you record something of width/height <= 0 or m_fps = 0";
        }
        else
        {
            qInfo() << "Started Streaming";
            m_streaming_started = true;
        }
        m_block_size = 0;
    }
    else // stream images
    {
        quint32 no_of_blocks, frame_id;

        *m_in_datastream >> frame_id >> no_of_blocks;
        QPoint position;
        QImage image;

        // TODO save to own format file.
        QString dir = QString("%1/%2.jpg").arg(m_dir_save);
        for (quint32 i = 0; i < no_of_blocks; i++)
        {
            *m_in_datastream >> position >> image;
            int x = position.x();
            int y = position.y();
            for (int i = 0; i < image.width(); i++)
            {
                for (int j = 0; j < image.height(); j++)
                {
                    m_previous_frame->setPixel(i + x, j + y, image.pixel(i, j));
                }
            }
        }

        bool saved = m_previous_frame->save(dir.arg(m_current_frame_id, 6, 10, QChar('0')));
        if (!saved)
            qInfo() << "Can't save frame" << m_current_frame_id;

        if (constants::DEBUG_NETWORK)
            qInfo() << "Received m_current_frame_id = " << m_current_frame_id << ", network_frame_id = " << frame_id;

        m_block_size = 0;
        m_current_frame_id++;
    }

    // newline
    qInfo() << "";

    // try to consume the next frame too
    readyRead();
}

void StreamingServer::disconnected()
{
    qInfo() << "Disconnected";
    m_tcp_socket->deleteLater();
    m_tcp_socket        = nullptr;
    m_in_datastream     = nullptr;
    m_streaming_started = false;
}
