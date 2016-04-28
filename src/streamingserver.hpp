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
    void readyRead();
    void disconnected();

private:
    QString m_dir_save = "screenshots_network";

    bool m_streaming_started = false;
    QTcpServer* m_tcp_server = nullptr;
    QTcpSocket* m_tcp_socket;

    quint8 m_fps;
    qint16 m_screen_width;
    qint16 m_screen_height;
    QImage* m_previous_frame;
    qint16 m_current_frame_id = 0;

    quint8 m_expected_blocks;

    // the total size of the data block expected
    quint32 m_block_size               = 0;
    QNetworkSession* m_network_session = nullptr;
};

#endif // STREAMINGSERVER_H
