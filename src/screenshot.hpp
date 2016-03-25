#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QScreen>
#include <QDebug>
#include <QGuiApplication>

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

signals:
    void onScreenshot();

private:
    QPixmap m_pixmap;
};

#endif // SCREENSHOT_H
