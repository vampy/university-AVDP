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
    explicit Screenshot(QObject* parent = nullptr);
    QImage getImage() const;

    void setScreen(qint8 screen_id,
                   qint16 screen_x      = constants::DEFAULT_SCREEN_POS,
                   qint16 screen_y      = constants::DEFAULT_SCREEN_POS,
                   qint16 screen_width  = constants::DEFAULT_SCREEN_SIZE,
                   qint16 screen_height = constants::DEFAULT_SCREEN_SIZE);
    const QList<QScreen*> getScreens() const;

    void statsDisplay();
    void statsReset();

public slots:
    // Take a screenshot
    void take(int msec);
    void take();

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

    // Only different than the screen_width and screen_height above
    // if the image is not a multiple of the block width. They are used to grab the window.
    qint16 m_new_screen_width;
    qint16 m_new_screen_height;
    bool m_is_multiple = true;

    // For stats only
    quint32 m_stats_take_nr    = 0;
    quint32 m_stats_take_total = 0;
};

#endif // SCREENSHOT_HPP
