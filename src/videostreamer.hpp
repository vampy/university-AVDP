#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <QObject>
#include <QTcpSocket>
#include <QtNetwork>

#include "constants.hpp"
#include "imageblock.hpp"

class VideoStreamer : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamer(QObject* parent      = nullptr,
                           quint8 fps           = constants::DEFAULT_FPS2,
                           qint16 screen_width  = constants::DEFAULT_SCREEN_WIDTH,
                           qint16 screen_height = constants::DEFAULT_SCREEN_HEIGHT,
                           QString hostname     = constants::DEFAULT_HOSTNAME,
                           quint16 port         = constants::DEFAULT_PORT);
    //    ~VideoStreamer();
    void setConnectionInfo(QString hostname, quint16 port);
    void setResolution(qint16 width, qint16 height);
    void setFps(quint8 fps);

signals:
    void connected(bool);

public slots:
    void initConnection();
    void onSendFrame(QVector<Imageblock*>);

private slots:
    void sessionOpened();
    void socketError(QAbstractSocket::SocketError);

private:
    QTcpSocket* m_tcp_socket;
    quint8 m_fps;
    qint16 m_screen_width  = -1;
    qint16 m_screen_height = -1;

    QString m_hostname;
    quint16 m_port;

    QNetworkSession* m_network_session = nullptr;
    bool m_is_connected                = false;
};

#endif // VIDEOSTREAMER_H
