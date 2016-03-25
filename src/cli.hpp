#ifndef CLI_H
#define CLI_H

#include <QObject>
#include <QCommandLineParser>

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

signals:
    void finished();
};

#endif // CLI_H
