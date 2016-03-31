#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QScreen>
#include <QDebug>
#include <QGuiApplication>
#include <QTime>
#include "imageblock.hpp"
#include "constants.hpp"
#include "util.hpp"

class Screenshot : public QObject
{
    Q_OBJECT
public:
    Screenshot();

    // Take a screenshot
    void take(int msec);
    void take();

    QPixmap getPixmap() const;
    QImage getImage() const;

    void setScreenID(const qint8 &screen_id);
    QList<QScreen*> getScreens() const;

signals:
    void onScreenshot();

private:
    QPixmap m_pixmap;
    QImage m_image;

    // The screen we take the screenshot of, -1 means capture all
    qint8 m_screen_id = -1;
    qint16 m_capture_width = -1;
    qint16 m_capture_height = -1;

    QQueue<QImage>* m_queue;
};

#endif // SCREENSHOT_H
