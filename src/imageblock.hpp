#ifndef IMAGEBLOCK_H
#define IMAGEBLOCK_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include "util.hpp"

class Imageblock : public QObject
{
    Q_OBJECT
public:
    Imageblock(int frameId, QPoint position, QImage image);
    bool isEqualTo(const Imageblock& other);
    QImage getImage() const;

private:
    int m_frameId;
    QPoint m_position;
    QImage m_image;
    double m_threshold = 5.0;
};

#endif // IMAGEBLOCK_H
