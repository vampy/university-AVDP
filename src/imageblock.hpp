#ifndef IMAGEBLOCK_H
#define IMAGEBLOCK_H

#include <QObject>
#include <QDebug>
#include <QImage>

class Imageblock  : public QObject
{
    Q_OBJECT
public:
    Imageblock(int frameId,QPoint position, QImage image);
    bool isEqualTo(const Imageblock& other);
    QImage getImage() const;

private:
    int m_frameId;
    QPoint m_position;
    QImage m_image;
};

#endif // IMAGEBLOCK_H