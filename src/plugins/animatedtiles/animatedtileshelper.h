#ifndef ANIMATEDTILESHELPER_H
#define ANIMATEDTILESHELPER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsWidget>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>

class Pixmap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    Pixmap(const QPixmap &pix, QGraphicsItem* parent)
        : QObject(), QGraphicsPixmapItem(pix, parent)
    {
        setCacheMode(DeviceCoordinateCache);
    }
};

class Button : public QGraphicsWidget
{
    Q_OBJECT
public:
    Button(const QPixmap &pixmap, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), _pix(pixmap)
    {
        setAcceptHoverEvents(true);
        setCacheMode(DeviceCoordinateCache);
    }

    QRectF boundingRect() const
    {
        return QRectF(-65, -65, 130, 130);
    }

    QPainterPath shape() const
    {
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
    {
        bool down = option->state & QStyle::State_Sunken;
        QRectF r = boundingRect();
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, option->state & QStyle::State_MouseOver ? Qt::white : Qt::lightGray);
        grad.setColorAt(down ? 0 : 1, Qt::darkGray);
        painter->setPen(Qt::darkGray);
        painter->setBrush(grad);
        painter->drawEllipse(r);
        QLinearGradient grad2(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, Qt::darkGray);
        grad.setColorAt(down ? 0 : 1, Qt::lightGray);
        painter->setPen(Qt::NoPen);
        painter->setBrush(grad);
        if (down)
            painter->translate(2, 2);
        painter->drawEllipse(r.adjusted(5, 5, -5, -5));
        painter->drawPixmap(-_pix.width()/2, -_pix.height()/2, _pix);
    }

signals:
    void pressed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) {
        emit pressed();
        update();
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
        update();
    }

private:
    QPixmap _pix;
};

#endif // ANIMATEDTILESHELPER_H
