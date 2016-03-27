#include "imageblock.hpp"

Imageblock::Imageblock(int frameId, QPoint position, QImage image)
    : m_frameId(frameId), m_position(position), m_image(image)
{
}

bool Imageblock::isEqualTo(const Imageblock& other)
{
    double mse = 0.0;
    for (int y = 0; y < m_image.height(); y++)
    {
        for (int x = 0; x < m_image.width(); x++)
        {
            mse += pow(util::yComponent(m_image.pixel(x, y)) - util::yComponent(other.getImage().pixel(x, y)), 2.0);
        }
    }
    mse = mse / (double)(m_image.width() * m_image.width());
    return mse < m_threshold;
}

QImage Imageblock::getImage() const { return m_image; }
