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

const qreal DEFAULT_FPS   = 25;
const quint8 DEFAULT_FPS2 = 25;

// Aka localhost
const char* const DEFAULT_HOSTNAME_CLIENT = "127.0.0.1";
const quint16 DEFAULT_PORT                = 12345;

// use the whole screen, DEFAULT_SCREEN_SIZE
const qint16 DEFAULT_SCREEN_WIDTH  = 0;
const qint16 DEFAULT_SCREEN_HEIGHT = 0;
const bool IS_NETWORKING           = true;
const bool DEBUG_NETWORK           = true;

// This should affect performance a lot
const quint8 DEFAULT_IMAGE_DIFF_SKIP_LINE   = 1;
const quint8 DEFAULT_IMAGE_DIFF_SKIP_COLUMN = 2;
const qreal DEFAULT_IMAGE_DIFF_THRESHOLD    = 5.0;
}

#endif // CONSTANTS_HPP
