#ifndef UTIL_HPP
#define UTIL_HPP

#include <QColor>
#include <QDebug>
#include <QDir>
#include <QImage>
#include "constants.hpp"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif

namespace util
{

inline void sleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000 * 1000};
    nanosleep(&ts, NULL);
#endif
}

// Round to then nearest multiple
inline int roundUp(int numToRound, int multiple)
{
    int remainder = numToRound % multiple;

    return remainder == 0 ? numToRound : (numToRound + multiple - remainder);
}

inline qint32 toInt32(const QString& string)
{
    bool ok;
    qint32 nr = string.toInt(&ok);
    Q_ASSERT(ok);

    return nr;
}

inline qreal toReal(const QString& string)
{
    bool ok;
    qreal nr = string.toDouble(&ok);
    Q_ASSERT(ok);

    return nr;
}

// See https://code.woboq.org/qt5/qtbase/src/gui/image/qimage.cpp.html#_ZN6QImage8setPixelEiij
// Set pixel without detach
inline void setPixel(const QImage& src_image, int x, int y, QRgb index_or_rgb)
{
    // No detach for you bits() ;)
    uchar* src_bits = const_cast<uchar*>(src_image.constBits());

    // get line y
    QRgb* line_y = reinterpret_cast<QRgb*>(src_bits + y * src_image.bytesPerLine());

    // Only support Format_RGB32
    line_y[x] = index_or_rgb;
}

// See https://code.woboq.org/qt5/qtbase/src/gui/image/qimage.cpp.html#_ZNK6QImage5pixelEii
inline QRgb getPixel(const QImage& image, int x, int y)
{
    uchar* line = const_cast<uchar*>(image.constBits()) + y * image.bytesPerLine();

    return reinterpret_cast<QRgb*>(line)[x];
}

bool createDir(QString, bool remove_if_exists = true);
void copyBlockFromImage(const QImage&, const QImage&, int, int);
void copyBlockColor(const QImage&, QRgb, int, int);
}

#endif // UTIL_HPP
