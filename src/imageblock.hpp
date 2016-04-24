#ifndef IMAGEBLOCK_HPP
#define IMAGEBLOCK_HPP

#include <QImage>
#include "util.hpp"

class Imageblock : public QObject
{
    Q_OBJECT
public:
    explicit Imageblock(quint32 frame_id, const QPoint& position, const QImage& image);
    QImage getImage() const;
    QPoint getPosition() const;

    bool operator==(const Imageblock& other);
    bool operator!=(const Imageblock& other);

private:
    quint32 m_frame_id;
    QPoint m_position;
    QImage m_image;
};

#endif // IMAGEBLOCK_HPP
