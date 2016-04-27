#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <QtGlobal>

namespace constants
{
const char* const APP_NAME        = "Video Streamer";
const char* const APP_VERSION     = "0.1";
const char* const APP_DESCRIPTION = "Video streamer for forest";
const quint8 BLOCK_WIDTH          = 16;

const qint8 DEFAULT_SCREEN = 0;

// see screenshot class for more details about these defaults
const qint16 DEFAULT_SCREEN_POS  = -1;
const qint16 DEFAULT_SCREEN_SIZE = 0;

const qreal DEFAULT_FPS                  = 25;
const quint8 DEFAULT_FPS2                = 25;
const qreal DEFAULT_IMAGE_DIFF_THRESHOLD = 5.0;

const char* const DEFAULT_HOSTNAME = "localhost";
const quint16 DEFAULT_PORT         = 12345;
const qint16 DEFAULT_SCREEN_WIDTH  = 1376;
const qint16 DEFAULT_SCREEN_HEIGHT = 768;
const bool IS_NETWORKING = false;
}

#endif // CONSTANTS_HPP
