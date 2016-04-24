#include "imageblock.hpp"

Imageblock::Imageblock(quint32 frame_id, const QPoint& position, const QImage& image)
    : m_frame_id(frame_id), m_position(position), m_image(image)
{
}

QImage Imageblock::getImage() const { return m_image; }

QPoint Imageblock::getPosition() const { return m_position; }

// Calculate luma https://en.wikipedia.org/wiki/Luma_%28video%29
#define YCOMPONENT(_color) (0.299 * ((_color >> 16) & 0xff) + 0.587 * ((_color >> 8) & 0xff) + 0.114 * (_color & 0xff))

bool Imageblock::operator==(const Imageblock& other)
{
    qreal mse         = 0;
    auto other_image  = other.getImage();
    auto current_qrgb = reinterpret_cast<QRgb*>(const_cast<uchar*>(m_image.constBits()));
    auto other_qrgb   = reinterpret_cast<QRgb*>(const_cast<uchar*>(other_image.constBits()));
    auto bytes_line   = m_image.bytesPerLine() / sizeof(QRgb);
    auto width        = m_image.width();
    auto height       = m_image.height();

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            auto index = x + y * bytes_line;
            auto value = YCOMPONENT(current_qrgb[index]) - YCOMPONENT(other_qrgb[index]);
            mse += value * value;
        }
    }
    mse /= (width * height);

    return mse < constants::DEFAULT_IMAGE_DIFF_THRESHOLD;
}

bool Imageblock::operator!=(const Imageblock& other) { return !(*this == other); }
