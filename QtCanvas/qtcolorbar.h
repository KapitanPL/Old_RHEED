#ifndef QTCOLORBAR_H
#define QTCOLORBAR_H

#include <QVector>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QColor>

class QtColorBar: public QGraphicsView
{
public:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmap;
    QImage *canvas;
    char msg[256];
    int width, height; //defines QtColorBar widget default dimensions
    QtColorBar(QWidget *parent=0);
    ~QtColorBar();
    unsigned long IndexMax;
    void setColorMap(char *mapName);
    void setRanges(double min, double max);
    QRgb returnColor(double input, int * refIndex = nullptr);
    QList<QGraphicsTextItem*> labels;
    int labelsLen;
private:
    int depth;
    double Rmin, Rspan;
    int colorMapIndex;
    void updateColors();
    void updateRanges();
};

#endif // QTCOLORBAR_H
