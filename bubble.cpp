#include "bubble.hpp"
#include "os.hpp"
#include <QPainter>
#include <QWidget>
#include "log.hpp"

Bubble::Bubble(Seat orientation) :
    QObject(),
    QGraphicsItem(),
    m_offset(67,39.5),
    m_bubble(os::GFX_PATH + "bubble.png"),
    m_text("10 No Trumps"),
    m_text_yo(0),
    m_font("sans", 12, 3)
{
    trace;
    setScale(0);
    switch(orientation) {
    case WEST:
        m_bubble = m_bubble.transformed(QTransform::fromScale(-1,1));
        break;
    case EAST:
        m_bubble = m_bubble.transformed(QTransform::fromScale(1,-1));
        m_text_yo = 15;
        break;
    case NORTH:
        m_bubble = m_bubble.transformed(QTransform::fromScale(-1,-1));
        m_text_yo = 15;
        break;
    case SOUTH:
        break;
    }
}

void Bubble::setText(const QString& str) {
    trace;
    m_text = str;
}

QRectF Bubble::boundingRect() const {
    trace;
    return QRectF(m_bubble.rect().topLeft()-m_offset, m_bubble.rect().size());
}

void Bubble::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget) {
    painter->setFont(m_font);
    painter->drawPixmap(-m_offset.x(),-m_offset.y(), m_bubble);
    QRectF rect = painter->boundingRect(widget->rect(), m_text);
    painter->drawText(-rect.width()/2, m_text_yo, m_text);
}

