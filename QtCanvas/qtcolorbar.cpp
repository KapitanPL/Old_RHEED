#include "qtcolorbar.h"
#include <QSizePolicy>
#include <cmath>
#include <QMessageBox>
#include <QDebug>


QtColorBar::QtColorBar(QWidget *parent)
{
    width=50;
    height=500;
    this->setFrameShadow(QFrame::Raised);
    int heightPlus=15;
    scene = new QGraphicsScene(parent);
    this->setScene(scene);
    canvas = new QImage(width,height,QImage::Format_ARGB32);
    pixmap = new QGraphicsPixmapItem(QPixmap::fromImage(*canvas));
    scene->addItem(pixmap);
    QRectF scenR = scene->sceneRect();
    scenR.setWidth(scenR.width()+50);
    scene->setSceneRect(scenR);
    colorMapIndex=0;
    IndexMax=255; //8bit color resolution
    labelsLen=5;
    for (int i=0; i<labelsLen; i++){
        QGraphicsTextItem *label = new QGraphicsTextItem;
        label->setPlainText("0");
        label->setPos(width,((labelsLen-1-i)*height/float(labelsLen))-label->boundingRect().height()/2);
        scene->addItem(label);
        labels.append(label);
    }
    this->setFixedSize(scenR.width(),scenR.height()+2*heightPlus);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setRanges(0.0,1.0);
    this->setColorMap("grayscale");
}

QtColorBar::~QtColorBar(){
}

void QtColorBar::setColorMap(char* mapName){
    std::string smapName= mapName;
    if (smapName=="grayscale"){
        this->colorMapIndex=0;
    }
    else if (smapName=="greenblue"){
        this->colorMapIndex=1;
    }
    else if (smapName=="yellowblack"){
        this->colorMapIndex=2;
    }
    else{
        this->colorMapIndex=0;
    }
    this->updateColors();
}

void QtColorBar::updateColors(){
    for (int i=0; i<this->height; i++){
        double val = double(i)/this->height*this->Rspan;
        QRgb color = this->returnColor(val);
        for (int j=0; j<this->width; j++){
            this->canvas->setPixel(j,this->height-i-1,color);
        }
    }
    this->pixmap->setPixmap(QPixmap::fromImage(*this->canvas));

}

void QtColorBar::setRanges(double min, double max){
    if (max>min){
        this->Rmin=min;
        this->Rspan=(max-min);
        this->updateRanges();
    }
}

void QtColorBar::updateRanges(){
    double val;
    char valText[256];
    for (int i=0; i<labels.length(); i++){
        val=this->Rmin+i*this->Rspan/double(labels.length()-1);
        if (abs(val)>1000){
            sprintf_s(valText,256,"%.3e",val);
        }
        else{
            sprintf_s(valText,256,"%.3f",val);
        }
        labels[i]->setPlainText(valText);
    }
}

QRgb QtColorBar::returnColor(double input, int * refIndex){
    double val;
    val=(input-this->Rmin)/this->Rspan;
    int index = static_cast<int>(val*this->IndexMax+0.5);
    if (refIndex)
        *refIndex = index;
    switch (this->colorMapIndex){
    case 0: //grayscale
        return qRgba(index,index,index,255);
    case 1: //"greenblue"
        return qRgba(0,index,255-index,255);
    case 2: //yellowblack
        return qRgba(index,index,0,255);
    default: //grayscale
        return qRgba(index,index,index,255);
    }
}
