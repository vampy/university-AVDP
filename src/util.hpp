#ifndef UTIL_H
#define UTIL_H
#include <QColor>
namespace util
{
// Round to then nearest multiple
inline int roundUp(int numToRound, int multiple)
{
    int remainder = numToRound % multiple;

    return remainder == 0 ? numToRound : (numToRound + multiple - remainder);
}
inline double yComponent(QRgb color)
{
    double yComponent = 0.299 * qRed(color) + 0.587 * qGreen(color) + 0.114 * qBlue(color);
    return yComponent;
}
}

#endif // UTIL_H
