#include <QApplication>
#include <QDesktopWidget>

#include "constants.hpp"
#include "gui.hpp"
#include "cli.hpp"

int main(int argc, char* argv[])
{
    // Warmup app
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName(constants::APP_NAME);
    QApplication::setApplicationName(constants::APP_NAME);
    QApplication::setApplicationVersion(constants::APP_VERSION);

    // Parse command line
    QCommandLineParser parser;
    parser.setApplicationDescription(constants::APP_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();

    // define arguments
    Q_ASSERT(parser.addOptions({// Show the gui or command line
        {{"g", "gui"}, "GUI mode"}}));

    // Process the actual command line arguments given by the user
    parser.process(app);

    GUI* gui;
    CLI* cli;
    if (parser.isSet("gui"))
    {
        // gui mode
        qDebug("GUI mode");
        gui = new GUI();
        gui->show();
    }
    else
    {
        // cli mode
        qDebug("Console mode");
        cli = new CLI(&app, parser);

        // quit app when finished signal  received
        QObject::connect(cli, &CLI::finished, &app, &QApplication::quit);

        // launch cli
        QTimer::singleShot(0, cli, &CLI::run);
    }

    //    QApplication::desktop()->grab()

    return app.exec();
}
