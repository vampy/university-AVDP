#ifndef UTIL_H
#define UTIL_H

namespace util
{
// Round to then nearest multiple
inline int roundUp(int numToRound, int multiple)
{
    int remainder = numToRound % multiple;

    return remainder == 0 ? numToRound : (numToRound + multiple - remainder);
}
}

#endif // UTIL_H
