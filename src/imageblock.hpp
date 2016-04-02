#ifndef IMAGEBLOCK_HPP
#define IMAGEBLOCK_HPP

#include <QImage>
#include "util.hpp"
#include "constants.hpp"

class Imageblock : public QObject
{
    Q_OBJECT
public:
    explicit Imageblock(int frameId, QPoint position, QImage image);
    QImage getImage() const;

    bool operator==(const Imageblock& other);
    bool operator!=(const Imageblock& other);

private:
    int m_frameId;
    QPoint m_position;
    QImage m_image;
};

#endif // IMAGEBLOCK_HPP
