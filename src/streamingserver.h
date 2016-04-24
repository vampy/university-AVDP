#ifndef STREAMINGSERVER_H
#define STREAMINGSERVER_H

#include <QMessageBox>
#include <QObject>
#include <QtNetwork>

#include "util.hpp"

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
    QImage* m_previous_frame;
    qint16 m_current_frame_id = 0;

    quint8 m_expected_blocks;
    quint32 m_block_size = 0;
    QNetworkSession* m_network_session;

};

#endif // STREAMINGSERVER_H
