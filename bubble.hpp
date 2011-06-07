#ifndef BUBBLE_HPP
#define BUBBLE_HPP

#include <QGraphicsItem>
#include <QObject>
#include <QFont>
#include "seat.hpp"

class Bubble : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    Bubble(Seat orientation);
    void setText(const QString& str);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    static const int X_MID = 67;
    static const int Y_MID = 26;

protected:

    QPoint m_offset;
    QPixmap m_bubble;
    QString m_text;
    unsigned m_text_yo;
    QFont m_font;
};

#endif // BUBBLE_HPP
