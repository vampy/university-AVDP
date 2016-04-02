#ifndef SCREENSHOT_HPP
#define SCREENSHOT_HPP

#include <QDebug>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTime>
#include <QTimer>
#include "constants.hpp"
#include "imageblock.hpp"
#include "util.hpp"

class QObject;

class Screenshot : public QObject
{
    Q_OBJECT
public:
    explicit Screenshot(qint8 screen_id      = constants::DEFAULT_SCREEN,
                        qint16 screen_x      = constants::DEFAULT_SCREEN_POS,
                        qint16 screen_y      = constants::DEFAULT_SCREEN_POS,
                        qint16 screen_width  = constants::DEFAULT_SCREEN_SIZE,
                        qint16 screen_height = constants::DEFAULT_SCREEN_SIZE);

    // Take a screenshot
    void take(int msec);
    void take();

    QImage getImage() const;

    void setScreen(qint8 screen_id,
                   qint16 screen_x      = constants::DEFAULT_SCREEN_POS,
                   qint16 screen_y      = constants::DEFAULT_SCREEN_POS,
                   qint16 screen_width  = constants::DEFAULT_SCREEN_SIZE,
                   qint16 screen_height = constants::DEFAULT_SCREEN_SIZE);
    QList<QScreen*> getScreens() const;

signals:
    void onScreenshot();

private:
    QImage m_image;

    // The screen we take the screenshot of
    QScreen* m_screen = nullptr;

    // will default to m_screen->geometry().x()/y()
    qint16 m_screen_x = constants::DEFAULT_SCREEN_POS;
    qint16 m_screen_y = constants::DEFAULT_SCREEN_POS;

    // will default to m_scren->geometry().height()/width()
    qint16 m_screen_width  = constants::DEFAULT_SCREEN_SIZE;
    qint16 m_screen_height = constants::DEFAULT_SCREEN_SIZE;

    QQueue<QImage>* m_queue;
};

#endif // SCREENSHOT_HPP
