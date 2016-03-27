#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QScreen>
#include <QDebug>
#include <QGuiApplication>
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
    void setPixmap(const QPixmap& pixmap);

    QImage getImage() const;
    void setImage(const QImage& image);

signals:
    void onScreenshot();

private:
    QPixmap m_pixmap;
    QImage m_image;

    QQueue<QImage>* m_queue;
};

#endif // SCREENSHOT_H
