#ifndef STREAMINGSERVER_H
#define STREAMINGSERVER_H

#include <QMessageBox>
#include <QObject>
#include <QtNetwork>

class StreamingServer : public QObject
{
    Q_OBJECT
public:
    explicit StreamingServer(QObject* parent = 0);

    ~StreamingServer();

signals:

public slots:

private slots:
    void establishConnection();
    void sessionOpened();
    void readData();

private:
    bool m_streaming_started = false;
    QTcpServer* m_tcp_server;
    QTcpSocket* m_tcp_socket;

    quint8 m_fps;
    qint16 m_screen_width;
    qint16 m_screen_height;

    quint8 m_expected_blocks;
    quint8 m_block_size;
    QNetworkSession* m_network_session;
};

#endif // STREAMINGSERVER_H
