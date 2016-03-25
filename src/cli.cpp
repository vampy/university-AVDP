#include "cli.hpp"

CLI::CLI(QCommandLineParser& parser) {}

void CLI::quit() { emit finished(); }

void CLI::run()
{
    qDebug("CLI::run");
    quit();
}
