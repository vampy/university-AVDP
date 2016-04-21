#ifndef UTIL_HPP
#define UTIL_HPP

#include <QColor>
#include <QDebug>
#include <QImage>
#include "constants.hpp"

namespace util
{
// Round to then nearest multiple
inline int roundUp(int numToRound, int multiple)
{
    int remainder = numToRound % multiple;

    return remainder == 0 ? numToRound : (numToRound + multiple - remainder);
}

// used the definitions qRed and qGreen here, I know
inline double yComponent(QRgb color)
{
    return 0.299 * ((color >> 16) & 0xff) + 0.587 * ((color >> 8) & 0xff) + 0.114 * (color & 0xff);
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

void copyBlock(const QImage& dst_image, const QImage& block, int start_x, int start_y);
void copyBlockColor(const QImage& dst_image, QRgb color, int start_x, int start_y);
}

#endif // UTIL_HPP
