#include "util.hpp"

namespace util
{

bool createDir(QString dirname, bool remove_if_exists)
{
    // remove the dir if it does exist
    QDir dir(dirname);
    if (remove_if_exists && dir.exists())
    {
        if (!dir.removeRecursively())
        {
            qCritical() << QString("Can't remove dir %1").arg(dirname);
            return false;
        }
    }

    // go up from the dir so that we can create it
    if (!dir.cdUp())
    {
        qCritical() << QString("Can't cd up from dir %1").arg(dirname);
        return false;
    }

    if (!dir.mkdir(dirname))
    {
        qCritical() << QString("Can't mkdir dir %1").arg(dirname);
        return false;
    }

    return true;
}

// See https://code.woboq.org/qt5/qtbase/src/gui/image/qimage.cpp.html#_ZN6QImage8setPixelEiij
// Copy the block located at position `block_start_x, block_start_y` from `src_image` to `dst_image` at the same position.
void copyBlockFromImage(const QImage& dst_image, const QImage& src_image, int block_start_x, int block_start_y)
{
    // No detach for you bits() ;)
    auto dst_bits   = const_cast<uchar*>(dst_image.constBits());
    auto bytes_line = dst_image.bytesPerLine();
    auto src_bits   = const_cast<uchar*>(src_image.constBits());

    // Assume that we copy from one image to another with the same size
    if (bytes_line != src_image.bytesPerLine())
    {
        qWarning() << "util::copyBlockFromImage: " << bytes_line << "!=" << src_image.bytesPerLine();
        return;
    }

    // block_start_x and block_start_y are relative to a QRgb (4 bytes) image,
    // convert so that they are relative to uchar image (1 byte)
    // NOTE: start_y does not need to be converted
    block_start_x *= sizeof(QRgb);

    auto dst_pos = dst_bits + block_start_x;
    auto src_pos = src_bits + block_start_x;

    // iterate over lines
    for (auto i = 0; i < constants::BLOCK_WIDTH; i++)
    {
        auto line_pos = (i + block_start_y) * bytes_line;

        // copy line by line
        memcpy(dst_pos + line_pos, src_pos + line_pos, sizeof(QRgb) * constants::BLOCK_WIDTH);
    }
}

// Set a block with color `set_color` at position `start_x, start_y` in `dst_image`
void copyBlockColor(const QImage& dst_image, QRgb set_color, int start_x, int start_y)
{
    // use QRgb
    auto dst_bits   = reinterpret_cast<QRgb*>(const_cast<uchar*>(dst_image.constBits()));
    auto bytes_line = dst_image.bytesPerLine() / sizeof(QRgb);

    // start from column x
    auto dst_start = dst_bits + start_x;

    // iterate over lines
    for (auto i = 0; i < constants::BLOCK_WIDTH; i++)
    {
        // line y = (i + start_y) * bytes_line
        // column x on line y = start_x
        std::fill_n(dst_start + (i + start_y) * bytes_line, constants::BLOCK_WIDTH, set_color);

        // DOES NOT WORK because it uses conversion to unsigned char, sigh
        // memset(dst_start + (i + start_y) * bytes_line, set_color, sizeof(QRgb) * constants::BLOCK_WIDTH);
    }
}
}
