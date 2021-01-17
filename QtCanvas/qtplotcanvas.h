#ifndef QTPLOTCANVAS_H
#define QTPLOTCANVAS_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QImage>
#include <QMessageBox>
#include <QtCanvas/qtcolorbar.h>

void checkBuffer(double * bufferIn, unsigned length);


class QtPlotCanvas:  public QWidget
{
    Q_OBJECT

public:
    QHBoxLayout *mainH;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmap;
    QImage *canvas;
    char msg[256];
    int width, height;
    QtColorBar *colorBar;
    double * doubleMatrix;
    int dimx, dimy;
    int showCB;
    void resizePlot(int x, int y);
    void plotMatrix(double* inMatrix,int xsize, int ysize);
    void setupUI(QWidget *parent, qreal tilt);
    void togleColorBar();
    QtPlotCanvas(int w=494, int h=656, qreal tilt=0.0);
    ~QtPlotCanvas();
    int iCounter = 0;
};

#endif // QTPLOTCANVAS_H
