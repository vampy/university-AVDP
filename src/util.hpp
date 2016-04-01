#ifndef UTIL_HPP
#define UTIL_HPP

#include <QColor>

namespace util
{
// Round to then nearest multiple
inline int roundUp(int numToRound, int multiple)
{
    int remainder = numToRound % multiple;

    return remainder == 0 ? numToRound : (numToRound + multiple - remainder);
}

// Calculate luma https://en.wikipedia.org/wiki/Luma_%28video%29
inline double yComponent(QRgb color) { return 0.299 * qRed(color) + 0.587 * qGreen(color) + 0.114 * qBlue(color); }
}

#endif // UTIL_HPP
