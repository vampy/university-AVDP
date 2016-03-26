#ifndef CLI_H
#define CLI_H

#include <QObject>
#include <QCommandLineParser>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include "screenshot.hpp"

// see https://stackoverflow.com/questions/4180394/how-do-i-create-a-simple-qt-console-application-in-c
// and http://treyweaver.blogspot.ro/2013/02/qt-console-application-template-tutorial.html
class CLI : public QObject
{
    Q_OBJECT

public:
    CLI(QObject* parent, QCommandLineParser& parser);

    void quit();

public slots:
    void run();

private slots:
    void takeScreenshot();

signals:
    void finished();

private:
    Screenshot* m_screenshot;
    const QString m_dir_screenshots = "screenshots";
    quint64 m_counter = 0;
    QTimer* m_timer;
    QTime m_elapsed;
};

#endif // CLI_H
