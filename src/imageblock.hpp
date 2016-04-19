#ifndef IMAGEBLOCK_HPP
#define IMAGEBLOCK_HPP

#include <QImage>
#include "constants.hpp"
#include "util.hpp"

class Imageblock : public QObject
{
    Q_OBJECT
public:
    explicit Imageblock(quint32 frame_id, QPoint position, QImage image);
    QImage getImage() const;

    bool operator==(const Imageblock& other);
    bool operator!=(const Imageblock& other);

private:
    quint32 m_frame_id;
    QPoint m_position;
    QImage m_image;
};

#endif // IMAGEBLOCK_HPP
