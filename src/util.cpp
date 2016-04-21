#include "util.hpp"

namespace util
{
//// See https://code.woboq.org/qt5/qtbase/src/gui/image/qimage.cpp.html#_ZN6QImage8setPixelEiij
//// Set pixel without detach
void copyBlock(const QImage& dst_image, const QImage& block, int start_x, int start_y)
{
    // No detach for you bits() ;)
    auto dst_bits   = const_cast<uchar*>(dst_image.constBits());
    auto bytes_line = dst_image.bytesPerLine();
    auto block_bits = const_cast<uchar*>(block.constBits());
    if (dst_image.bytesPerLine() != block.bytesPerLine())
    {
        qWarning() << "util::copyBlock: dst_image.bytesPerLine() == block.bytesPerLine()";
        return;
    }

    //    auto dst_pos = dst_bits + start_x;
    //    auto src_pos = block_bits + start_x;
    //    auto offset = constants::BLOCK_WIDTH * sizeof(QRgb);
    for (int i = 0; i < constants::BLOCK_WIDTH; i++)
    {
        //        auto line = (i + start_y) * bytes_line;
        auto dst_line = dst_bits + (i + start_y) * bytes_line;
        auto src_line = block_bits + (i + start_y) * bytes_line;

        for (int j = 0; j < constants::BLOCK_WIDTH; j++)
        {
            //            auto pos = start_x + j + (i + start_y) * bytes_line;
            //            Q_ASSERT(pos < dst_image.byteCount());
            reinterpret_cast<QRgb*>(dst_line)[start_x + j] = reinterpret_cast<QRgb*>(src_line)[start_x + j];
            //            dst_bits[pos] = block_bits[pos];
        }
        // copy line by line, TODO use memcpy
        //        memcpy(dst_pos + line, src_pos + line, offset);
    }
}

void copyBlockColor(const QImage& dst_image, QRgb color, int start_x, int start_y)
{
    // use QRgb
    auto dst_bits   = reinterpret_cast<QRgb*>(const_cast<uchar*>(dst_image.constBits()));
    auto bytes_line = dst_image.bytesPerLine() / sizeof(QRgb);

    for (int i = 0; i < constants::BLOCK_WIDTH; i++)
    {
        // line y = (i + start_y) * bytes_line
        // column x on line y = start_x
        //        std::fill_n(dst_bits + (i + start_y) * bytes_line + start_x, constants::BLOCK_WIDTH, color);

        // memset works with bytes, FIXME why black?
        //         memset(dst_bits + (i + start_y) * bytes_line + start_x, color, 4 * constants::BLOCK_WIDTH);
        for (int j = 0; j < constants::BLOCK_WIDTH; j++)
        {
            dst_bits[(i + start_y) * bytes_line + start_x + j] = color;
        }
    }
}
}
