#include "qchooserect.h"
#include <QPen>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include "Windows.h"
#include <QDebug>

QChooseRect::QChooseRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent, QGraphicsScene */*scene*/): QGraphicsRectItem(x,y,w,h,parent)
{
    this->setFlag(this->ItemIsMovable,true);
    this->setAcceptHoverEvents(true);
    this->cornerID.fill(false,4);
    this->sz = 6;
}

void QChooseRect::setPen(const QPen &pen){
    QGraphicsRectItem::setPen(pen);
}

void QChooseRect::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    QRectF position = this->boundingRect();
    QPen pen = this->pen();
    QBrush brush = QBrush(this->pen().color());
    this->UL = new QCornerRect(QRectF(position.topLeft().x(),position.topLeft().y(),sz,sz),this,this->scene());
    this->UR = new QCornerRect(QRectF(position.topRight().x()-sz,position.topRight().y(),sz,sz),this,this->scene());
    this->LL = new QCornerRect(QRectF(position.bottomLeft().x(),position.bottomLeft().y()-sz,sz,sz),this,this->scene());
    this->LR = new QCornerRect(QRectF(position.bottomRight().x()-sz,position.bottomRight().y()-sz,sz,sz),this,this->scene());
    this->UL->connect(this->UL,SIGNAL(mousePress(QGraphicsSceneMouseEvent*,qreal,qreal)),this,SLOT(startResizing(QGraphicsSceneMouseEvent*,qreal,qreal)));
    this->UR->connect(this->UR,SIGNAL(mousePress(QGraphicsSceneMouseEvent*,qreal,qreal)),this,SLOT(startResizing(QGraphicsSceneMouseEvent*,qreal,qreal)));
    this->LL->connect(this->LL,SIGNAL(mousePress(QGraphicsSceneMouseEvent*,qreal,qreal)),this,SLOT(startResizing(QGraphicsSceneMouseEvent*,qreal,qreal)));
    this->LR->connect(this->LR,SIGNAL(mousePress(QGraphicsSceneMouseEvent*,qreal,qreal)),this,SLOT(startResizing(QGraphicsSceneMouseEvent*,qreal,qreal)));
    this->UL->connect(this->UL,SIGNAL(mouseMoved(QGraphicsSceneMouseEvent*)),this,SLOT(resizing(QGraphicsSceneMouseEvent*)));
    this->UR->connect(this->UR,SIGNAL(mouseMoved(QGraphicsSceneMouseEvent*)),this,SLOT(resizing(QGraphicsSceneMouseEvent*)));
    this->LL->connect(this->LL,SIGNAL(mouseMoved(QGraphicsSceneMouseEvent*)),this,SLOT(resizing(QGraphicsSceneMouseEvent*)));
    this->LR->connect(this->LR,SIGNAL(mouseMoved(QGraphicsSceneMouseEvent*)),this,SLOT(resizing(QGraphicsSceneMouseEvent*)));
    this->UL->connect(this->UL,SIGNAL(mouseReleased()),this,SLOT(stopResizing()));
    this->UR->connect(this->UR,SIGNAL(mouseReleased()),this,SLOT(stopResizing()));
    this->LL->connect(this->LL,SIGNAL(mouseReleased()),this,SLOT(stopResizing()));
    this->LR->connect(this->LR,SIGNAL(mouseReleased()),this,SLOT(stopResizing()));
    this->UL->setPen(pen);
    this->UR->setPen(pen);
    this->LL->setPen(pen);
    this->LR->setPen(pen);
    this->UL->setBrush(brush);
    this->UR->setBrush(brush);
    this->LL->setBrush(brush);
    this->LR->setBrush(brush);
}

void QChooseRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    delete this->UL;
    delete this->UR;
    delete this->LL;
    delete this->LR;
}

void QChooseRect::startResizing(QGraphicsSceneMouseEvent *event, qreal x, qreal y){
    cornerID[0]=(this->LL->isUnderMouse());
    cornerID[1]=(this->UL->isUnderMouse());
    cornerID[2]=(this->UR->isUnderMouse());
    cornerID[3]=(this->LR->isUnderMouse());
    this->res = true;
}

void QChooseRect::resizing(QGraphicsSceneMouseEvent *event){
    if (this->res){
        this->prepareGeometryChange();
        qreal xp=this->boundingRect().x();
        qreal yp=this->boundingRect().y();
        qreal wd=this->boundingRect().width();
        qreal he=this->boundingRect().height();
        bool bValid = false;
        if (cornerID[3]){ //LR
            xp=this->boundingRect().x()+.5;
            yp=this->boundingRect().y()+.5;
            wd=event->pos().x()-this->boundingRect().x();
            he=event->pos().y()-this->boundingRect().y();
            bValid = true;
        }
        else if (cornerID[1]){ //UL
            xp=event->pos().x()-0.5;
            yp=event->pos().y();
            wd=this->boundingRect().width()-event->pos().x()+this->boundingRect().x();
            he=this->boundingRect().height()-event->pos().y()+this->boundingRect().y()-0.5;
            bValid = true;
        }
        else if (cornerID[0]){ //LL
            xp=event->pos().x();
            yp=this->boundingRect().y()+0.5;
            wd=this->boundingRect().width()+this->boundingRect().x()-event->pos().x()-0.5;
            he=event->pos().y()-this->boundingRect().y();
            bValid = true;
        }
        else if (cornerID[2]){ //UR
            xp=this->boundingRect().x()+0.5;
            yp=event->pos().y()-0.5;
            wd=event->pos().x()-this->boundingRect().x();
            he=this->boundingRect().height()+this->boundingRect().y()-event->pos().y();
            bValid = true;
        }
        if (wd>0 && he>0 && bValid){
            this->setRect(xp,yp,wd,he); //x,y are rotated!!! but Qt don't carre as I rotated view, not scene!
            this->UL->setRect(xp,yp,sz,sz);
            this->UR->setRect(xp+wd-sz,yp,sz,sz);
            this->LL->setRect(xp,yp+he-sz,sz,sz);
            this->LR->setRect(xp+wd-sz,yp+he-sz,sz,sz);
        }
    }
}

void QChooseRect::stopResizing(){
    this->res=false;
}

QChooseRect::~QChooseRect(){

}

QCornerRect::QCornerRect(QRectF rect, QGraphicsItem *parent, QGraphicsScene */*scene*/): QGraphicsRectItem(rect,parent){
}

QCornerRect::~QCornerRect(){}

void QCornerRect::mousePressEvent(QGraphicsSceneMouseEvent *event){
    qreal x = this->boundingRect().topLeft().x();
    qreal y = this->boundingRect().topLeft().y();
    emit mousePress(event,x,y);
}

void QCornerRect::mouseReleaseEvent(QGraphicsSceneMouseEvent */*event*/){
    emit mouseReleased();
}

void QCornerRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    emit mouseMoved(event);
}
