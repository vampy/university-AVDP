#include "imageblock.hpp"

Imageblock::Imageblock(int frameId, QPoint position, QImage image)
    : m_frameId(frameId), m_position(position), m_image(image)
{
}

bool Imageblock::isEqualTo(const Imageblock& other)
{
    double mse = 0.0;
    m_image.setPixel(1, 1, qRgb(1, 1, 1));
    for (int y = 0; y < m_image.height(); y++)
    {
        for (int x = 0; x < m_image.width(); x++)
        {
            mse += pow((qRed(m_image.pixel(x, y)) + qGreen(m_image.pixel(x, y)) + qBlue(m_image.pixel(x, y))) / 3.0
                    - (qRed(other.getImage().pixel(x, y)) + qGreen(other.getImage().pixel(x, y))
                          + qBlue(other.getImage().pixel(x, y))) / 3.0,
                2.0);
        }
    }
    mse = mse / (double)(m_image.width() * m_image.width());
    return mse < 500;
}

QImage Imageblock::getImage() const { return m_image; }
