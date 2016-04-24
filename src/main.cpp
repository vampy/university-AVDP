#include <QApplication>
#include <QDesktopWidget>

#include "cli.hpp"
#include "constants.hpp"
#include "gui.hpp"

#include "streamingserver.h"

int main(int argc, char* argv[])
{
    // Warmup app
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName(constants::APP_NAME);
    QApplication::setApplicationName(constants::APP_NAME);
    QApplication::setApplicationVersion(constants::APP_VERSION);
    qRegisterMetaType<QQueue<Imageblock*>>("QQueue<Imageblock*>");
    // Parse command line
    QCommandLineParser parser;
    parser.setApplicationDescription(constants::APP_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();

    // define arguments
    const char* const OPTION_GUI      = "gui";
    const char* const OPTION_SCREEN   = "screen";
    const char* const OPTION_SCREEN_X = "screen-x";
    const char* const OPTION_SCREEN_Y = "screen-y";
    const char* const OPTION_SCREEN_W = "screen-w";
    const char* const OPTION_SCREEN_H = "screen-h";
    const char* const OPTION_FPS      = "fps";
    // const char* const OPTION_LIST_SCREENS = "list-screens";
    auto temp = parser.addOptions(
        {// Show the gui or command line
         {{"g", OPTION_GUI}, "GUI mode"},
         {{"f", OPTION_FPS}, "Frames per second", "fps", QString::number(constants::DEFAULT_FPS)},
         {{"s", OPTION_SCREEN}, "Screen to record", "screen_id", QString::number(constants::DEFAULT_SCREEN)},
         {OPTION_SCREEN_X, "Screen X coordinate", "x", QString::number(constants::DEFAULT_SCREEN_POS)},
         {OPTION_SCREEN_Y, "Screen Y coordinate", "y", QString::number(constants::DEFAULT_SCREEN_POS)},
         {OPTION_SCREEN_W, "Screen width", "width", QString::number(constants::DEFAULT_SCREEN_WIDTH)},
         {OPTION_SCREEN_H, "Screen height", "height", QString::number(constants::DEFAULT_SCREEN_HEIGHT)}});
    Q_ASSERT(temp);

    // Process the actual command line arguments given by the user
    parser.process(app);
    auto fps       = util::toReal(parser.value(OPTION_FPS));
    auto screen_id = util::toInt32(parser.value(OPTION_SCREEN));
    auto screen_x  = util::toInt32(parser.value(OPTION_SCREEN_X));
    auto screen_y  = util::toInt32(parser.value(OPTION_SCREEN_Y));
    auto screen_w  = util::toInt32(parser.value(OPTION_SCREEN_W));
    auto screen_h  = util::toInt32(parser.value(OPTION_SCREEN_H));

    GUI* gui;
    CLI* cli;
    if (parser.isSet(OPTION_GUI))
    {
        // gui mode
        qInfo() << "GUI mode";
//        gui = new GUI(fps, screen_id, screen_x, screen_y, constants::DEFAULT_SCREEN_WIDTH, constants::DEFAULT_SCREEN_HEIGHT);
        gui = new GUI(fps, screen_id, screen_x, screen_y, screen_w, screen_h);

        gui->show();
    }
    else
    {
        // cli mode
        qInfo() << "Console mode";
        cli = new CLI(&app);

        // quit app when finished signal  received
        QObject::connect(cli, &CLI::finished, &app, &QApplication::quit);

        // launch cli
        QTimer::singleShot(0, cli, &CLI::run);
    }

    StreamingServer* server=  new StreamingServer();

    return app.exec();
}
