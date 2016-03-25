#include "cli.hpp"

CLI::CLI(QObject* parent, QCommandLineParser& parser) : QObject(parent) {}

void CLI::quit() { emit finished(); }

void CLI::run()
{
    qDebug("CLI::run");
    quit();
}
