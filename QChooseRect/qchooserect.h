#ifndef QCHOOSERECT_H
#define QCHOOSERECT_H

#include <QGraphicsRectItem>
#include <QObject>

class QCornerRect: public QObject,public QGraphicsRectItem
{
   Q_OBJECT
public:
    QCornerRect(QRectF rect, QGraphicsItem *parent = 0, QGraphicsScene *scene =0);
    ~QCornerRect();
signals:
    void mousePress(QGraphicsSceneMouseEvent *event=0, qreal x=0, qreal y=0);
    void mouseReleased();
    void mouseMoved(QGraphicsSceneMouseEvent *event=0);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

class QChooseRect:public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    QCornerRect *UL,*UR, *LL, *LR;
    QChooseRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0, QGraphicsScene *scene =0);
    void setPen(const QPen &pen);
    ~QChooseRect();
    bool res;
    QVector<bool> cornerID;
public slots:
    void stopResizing();
    void resizing(QGraphicsSceneMouseEvent *event=0);
    void startResizing(QGraphicsSceneMouseEvent *event=0, qreal x=0, qreal y=0);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
    int sz;
};

#endif // QCHOOSERECT_H
