#include "qtplotcanvas.h"
#include <QStyle>
#include <cmath>

#include <QDebug>
#include <QFile>

#ifndef M_PI
#define M_PI 3.14159265
#endif

QtPlotCanvas::QtPlotCanvas(int w, int h, qreal tilt) : width(w),height(h)
{
    colorBar = NULL;
    this->showCB=1;
    setupUI(this,tilt);
}

QtPlotCanvas::~QtPlotCanvas(){

}

void QtPlotCanvas::setupUI(QWidget *parent, qreal tilt){
    mainH = new QHBoxLayout(parent);
    view = new QGraphicsView(parent);
    view->setFrameShadow(QFrame::Raised);
    mainH->addWidget(view);
    scene = new QGraphicsScene(view);
    canvas = new QImage(width,height,QImage::Format_ARGB32);
    pixmap = new QGraphicsPixmapItem(QPixmap::fromImage(*canvas));
    scene->addItem(pixmap);
    view->setScene(scene);
    view->rotate(tilt);
    view->setFixedSize(int(abs(sin(tilt*M_PI/180))*height+abs(cos(tilt*M_PI/180))*width),int(abs(cos(tilt*M_PI/180))*height+abs(sin(tilt*M_PI/180))*width));
    colorBar =  new QtColorBar(parent);
    if (this->showCB!=0){
        mainH->addWidget(colorBar);}
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void QtPlotCanvas::togleColorBar(){
    if (this->showCB==0){
        this->showCB=1;
        mainH->addWidget(colorBar);}
    else{
        this->showCB=0;
        mainH->removeWidget(colorBar);
    }
}

void QtPlotCanvas::resizePlot(int x, int y){
    this->dimx=x;
    this->dimy=y;
    this->view->setFixedSize(x,y);
    delete this->canvas;
    this->canvas = new QImage(x,y,QImage::Format_ARGB32);
    this->pixmap->setPixmap(QPixmap::fromImage(*this->canvas));
}

void QtPlotCanvas::plotMatrix(double* inMatrix,int xsize, int ysize){
    doubleMatrix = inMatrix;
    if (xsize!=canvas->width() || ysize!=canvas->height()){
        this->resizePlot(xsize, ysize);
        }
    double maxVal = inMatrix[0];
    double minVal = inMatrix[0];
    for (int i=0; i<xsize*ysize; i++){
        if (inMatrix[i]>maxVal){maxVal=inMatrix[i];}
        if (inMatrix[i]<minVal){minVal=inMatrix[i];}
    }
    this->colorBar->setRanges(minVal,maxVal);
    QRgb color;
    int rm,dv;
    unsigned buffLen = xsize*ysize;
    checkBuffer(inMatrix,buffLen);
    bool bAllBlack = true;
    int iRef = 0;
    for (unsigned i=0;i<buffLen;i++){
        rm=i%xsize;
        dv=i/xsize;
        color = colorBar->returnColor(inMatrix[i], &iRef);
        if (iRef != 0)
            bAllBlack = false;
        //qDebug() << inMatrix[i] << ", " << color;
        this->canvas->setPixel(rm,dv,color);
    }
    if (bAllBlack)
        qDebug() << "AllBlack!!!";
    this->pixmap->setPixmap(QPixmap::fromImage(*this->canvas));

    /*int iter = 0;
    QString sFileName = QString("C:/archive/PoStepanovi/dbg/file%1.png").arg(iter, 4, 10, QLatin1Char('0'));
    while (QFile::exists(sFileName))
    {
        sFileName = QString("C:/archive/PoStepanovi/dbg/file%1.png").arg(iter, 4, 10, QLatin1Char('0'));
        ++iter;
    }
    this->pixmap->pixmap().save(sFileName);
    qDebug() << "view updated: " << iCounter;
    iCounter++;*/
    QRectF sceneRect = this->scene->sceneRect();
    this->scene->update(sceneRect);
    this->view->update();
}

void checkBuffer(double *bufferIn, unsigned length)
{
    return;
    if (!bufferIn)
        return;
    double mean = 0;
    double dataMin = 0;
    double dataMax = 0;
    double min = 0;
    double max = double(unsigned (1 << 16));
    for (unsigned i = 0; i < length; ++i)
    {
        if (i == 0)
        {
            dataMin = bufferIn[i];
            dataMax = bufferIn[i];
        }
        else
        {
            dataMin = std::min(dataMin,bufferIn[i]);
            dataMax = std::max(dataMax,bufferIn[i]);
        }
        mean += bufferIn[i];
        if ((bufferIn[i] < min) || (bufferIn[i] > max))
        {
            qDebug() << bufferIn[i];
        }
    }
    mean /= length;
    qDebug() << "Buffer mean: " << mean;
    qDebug() << "Buffer min: " << dataMin;
    qDebug() << "Buffer max: " << dataMax;
}
