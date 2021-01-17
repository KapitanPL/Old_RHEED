#ifndef QRHEEDANAL_H
#define QRHEEDANAL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
# include <QCheckBox>
#include <QCustomPlot/qcustomplot.h>
#include <QObject>
# include "../myQtUtils.h"
//# include "../QChooseRect/qchooserect.h"

class QRheedAnal:public QWidget{
    Q_OBJECT
public:
    QRheedAnal(QCPDataMap *datas);
    ~QRheedAnal();
    QLineEdit *EFreq, *EAmp, *EPhase, *EOffset, *ESlope, *EDamping;
    QCPGraph *mainPlot,*leftB,*rightB,*fitPlot, *movingBorder;
    QVector<double> ranges;
    QCheckBox *automodel;
    double Lbound, Rbound;
public slots:
    void Bselected(QCPAbstractPlottable *item, QMouseEvent *event);
    void Bmove(QMouseEvent *event);
    void fitClicked();
    void mousePressed(QMouseEvent *ev);
    void mouseMoving(QMouseEvent *ev);
    void mouseReleased(QMouseEvent *ev);
signals:
    void frequenciEmit(double);
private:
    void setupUI();
    void setupControls(QHBoxLayout *lay);
    void setupFitControls(QVBoxLayout *lay);
    void zoomBox(QPointF TL, QPointF BR);
    QCustomPlot *plot;
    QCPItemRect *zoomIn;
    bool mouseDown;
};

#endif // QRHEEDANAL_H
