#include "qrheedanal.h"
#include <QLabel>
# include "fitter/fitter.h"
#include "math.h"



#ifndef M_PI
#define M_PI 3.141592
#endif


QRheedAnal::QRheedAnal(QCPDataMap *datas)
{
    this->fitPlot=NULL;
    int imin=0;
    int imax =0;
    this->movingBorder=NULL;
    this->setupUI();
    mainPlot = this->plot->addGraph();
    mainPlot->setData(datas);
    for (int i =0; i<datas->values().count(); i++){
        if (datas->values().value(i).value<datas->values().value(imin).value){imin=i;}
        else if (datas->values().value(i).value>datas->values().value(imax).value){imax=i;}
    }
    Lbound = datas->keys().first();
    Rbound = datas->keys().last();
    ranges.append(datas->values().value(imin).value);
    ranges.append(datas->values().value(imax).value);
    leftB = this->plot->addGraph();
    leftB->setData(QVector<double>(2,Lbound),ranges);
    leftB->setPen(QPen(Qt::black));
    leftB->setSelectable(true);
    //leftB->connect(leftB,SIGNAL(selectionChanged(bool)),this,SLOT(Bselected(bool)));
    rightB = this->plot->addGraph();
    rightB->setData(QVector<double>(2,Rbound),ranges);
    rightB->setPen(QPen(Qt::black));
    rightB->setSelectable(true);
    double span = Rbound - Lbound;
    this->plot->xAxis->setRange(Lbound-span/20,Rbound+span/20);
    this->plot->yAxis->setRange(ranges.at(0),ranges.at(1));
    //connect(plot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(Bselected(QMouseEvent*)));
    connect(plot,SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),this,SLOT(Bselected(QCPAbstractPlottable*,QMouseEvent*)));
    this->plot->replot();
    mouseDown=false;
    zoomIn = new QCPItemRect(this->plot);
    zoomIn->setPen(QPen(Qt::red));
    zoomIn->bottomRight->setType(QCPItemPosition::ptAbsolute);
    zoomIn->topLeft->setType(QCPItemPosition::ptAbsolute);
    this->plot->addItem(zoomIn);
}

void QRheedAnal::setupUI(){
    this->setWindowTitle("Ananlyse window");
    this->setGeometry(50,50,600,400);
    QHBoxLayout *MainH = new QHBoxLayout(this);
    this->plot=new QCustomPlot(this);
    MainH->addWidget(this->plot,1);
    this->plot->setInteraction(QCP::iRangeZoom,true);
    this->plot->connect(this->plot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(mousePressed(QMouseEvent*)));
    this->plot->connect(this->plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(mouseMoving(QMouseEvent*)));
    this->plot->connect(this->plot,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(mouseReleased(QMouseEvent*)));
    this->setupControls(MainH);
    this->show();
}

void QRheedAnal::setupControls(QHBoxLayout *lay){
    QVBoxLayout *layV = new QVBoxLayout();
    lay->addLayout(layV);
    this->setupFitControls(layV);
    automodel = new QCheckBox("Try to guess?");
    automodel->setChecked(true);
    layV->addWidget(automodel);
    QPushButton *fit = new QPushButton("Fit");
    connect(fit,SIGNAL(clicked()),this,SLOT(fitClicked()));
    layV->addWidget(fit);
    layV->addStretch(1);
}

void QRheedAnal::setupFitControls(QVBoxLayout *lay){
    QHBoxLayout *cols = new QHBoxLayout();
    lay->addLayout(cols);
    QVBoxLayout *cll = new QVBoxLayout();
    QVBoxLayout *clr = new QVBoxLayout();
    cols->addLayout(cll);
    cols->addLayout(clr);
    cll->addWidget(new QLabel("Frequency"));
    cll->addWidget(new QLabel("Amplitude"));
    cll->addWidget(new QLabel("Phase"));
    cll->addWidget(new QLabel("Offset"));
    cll->addWidget(new QLabel("Slope"));
    cll->addWidget(new QLabel("Damping"));
    EFreq = new QLineEdit("0.15");
    EFreq->setMaximumWidth(80);
    EAmp = new QLineEdit("1.0");
    EAmp->setMaximumWidth(80);
    EPhase =new QLineEdit("0.0");
    EPhase->setMaximumWidth(80);
    EOffset = new QLineEdit("0.0");
    EOffset->setMaximumWidth(80);
    ESlope = new QLineEdit("0.0");
    ESlope->setMaximumWidth(80);
    EDamping= new QLineEdit("-0.001");
    EDamping->setMaximumWidth(80);
    clr->addWidget(EFreq);
    clr->addWidget(EAmp);
    clr->addWidget(EPhase);
    clr->addWidget(EOffset);
    clr->addWidget(ESlope);
    clr->addWidget(EDamping);
    return;
}

void QRheedAnal::mousePressed(QMouseEvent *ev){
    if (ev->button()==1){
        zoomIn->topLeft->setCoords(ev->pos());
        zoomIn->bottomRight->setCoords(ev->pos());
        zoomIn->setVisible(true);
        this->mouseDown = true;
    }
    if (ev->button()==2){
        this->plot->rescaleAxes();
        this->plot->replot();
    }
    return;
}

void QRheedAnal::mouseMoving(QMouseEvent *ev){
    if (this->mouseDown){
        zoomIn->bottomRight->setCoords(ev->pos());
        this->plot->replot();
        return;
    }
    else return;
}

void QRheedAnal::mouseReleased(QMouseEvent *ev){
    if (ev->button()==1){
        this->mouseDown=false;
        QPointF TL = zoomIn->topLeft->coords();
        QPointF BR = zoomIn->bottomRight->coords();
        zoomIn->setVisible(false);
        QPointF Size = TL-BR;
        if (abs(Size.x())>5 && abs(Size.y())>5){
            this->zoomBox(TL,BR);
            this->plot->replot();}
    }
    return;
}

void QRheedAnal::zoomBox(QPointF TL, QPointF BR){
    double xmin = this->plot->xAxis->pixelToCoord(TL.x());
    double xmax = this->plot->xAxis->pixelToCoord(BR.x());
    double ymin = this->plot->yAxis->pixelToCoord(BR.y());
    double ymax = this->plot->yAxis->pixelToCoord(TL.y());
    this->plot->xAxis->setRange(xmin,xmax);
    this->plot->yAxis->setRange(ymin,ymax);
    this->plot->replot();
    return;
}

void QRheedAnal::Bselected(QCPAbstractPlottable* item, QMouseEvent* event){
    QCPGraph *iden = dynamic_cast<QCPGraph*>(item);
    if (iden == this->movingBorder){
        this->movingBorder = NULL;
        disconnect(plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(Bmove(QMouseEvent*)));
    }
    else{
        this->movingBorder = iden;
        connect(plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(Bmove(QMouseEvent*)));
    }
}

void QRheedAnal::Bmove(QMouseEvent* event){
    if (this->movingBorder==this->rightB){
        double x;
        x=this->plot->xAxis->pixelToCoord(event->pos().x());
        if (this->Lbound < x && x < this->plot->xAxis->range().upper){
            this->Rbound=x;
            this->movingBorder->setData(QVector<double>(2,x),ranges);
            this->plot->replot();
        }
    }
    else if (this->movingBorder==this->leftB){
        double x;
        x=this->plot->xAxis->pixelToCoord(event->pos().x());
        if (this->Rbound > x && x > this->plot->xAxis->range().lower){
            this->Lbound=x;
            this->movingBorder->setData(QVector<double>(2,x),ranges);
            this->plot->replot();
        }
    }
}

void QRheedAnal::fitClicked(){
    QVector<double> xdata = this->mainPlot->data()->keys().toVector();
    QVector<double> fitx;
    QVector<double> fity;
    std::vector<double> inigues(2);
    std::vector<double> outgues;
    double x1,x2,fx1,fx2;
    for (int i=0;i<xdata.size();++i){
        if (xdata[i]>=this->Lbound && xdata[i]<=this->Rbound){
            fitx.push_back(xdata[i]);
            fity.push_back(this->mainPlot->data()->values().value(i).value);
        }
    }
    if (this->fitPlot == NULL){
        this->fitPlot = this->plot->addGraph();
        this->fitPlot->setPen(QPen(Qt::green));
    }
    fitter model = fitter(fitx.toStdVector(),fity.toStdVector(),linear);
    if (this->automodel->isChecked()){    
        x1 = fitx.value(0);
        x2 = fitx.value(fitx.size()-1);
        fx1 = fity.value(0);
        fx2 = fity.value(fity.size()-1);
        inigues[1]=(fx1-fx2)/(x1-x2);
        inigues[0]=fx1 - (inigues[1]*x1);
        outgues = optimizeM(model,inigues);
        this->ESlope->setText(QString::number(outgues[1],'f',4));
        this->EOffset->setText(QString::number(outgues[0],'f',4));
        /* under construction */
        QVector<double> linearpart = QVector<double>::fromStdVector(linear(outgues,fitx.toStdVector()));
        QVector<double> substracted = vectorSubstract(&fity,&linearpart);
        QVector<double> zeros;
        double last = substracted.value(0);
        for (int i=1;i<substracted.size();++i){
            if (last*substracted.value(i)<0){
                zeros.append(fitx.value(i-1)+(fitx.value(i)-fitx.value(i-1))/2);
            }
            last=substracted.value(i);
        }
        double frGues=0.0;
        for (int i=1;i<zeros.size();++i){
            frGues+=zeros.value(i)-zeros.value(i-1);
        }
        frGues /= zeros.size()-1;
        frGues = 1/(2*frGues);
        double ampGues = (substracted[dvMax(&substracted)]-substracted[dvMin(&substracted)])/2;
        double phGues = asin((fx1-(inigues[0]+inigues[1]*x1))/ampGues)-(2*M_PI*frGues*x1);
        inigues.resize(6);
        outgues.resize(6);
        inigues[0]=outgues[0];
        inigues[1]= outgues[1];
        inigues[2]=this->EDamping->text().toDouble();
        inigues[3]=frGues;
        inigues[4]=phGues;
        inigues[5]=ampGues;
        model = fitter(fitx.toStdVector(),fity.toStdVector(),oscilations);
        outgues = optimizeM(model, inigues);
        this->ESlope->setText(QString::number(outgues[1],'f',4));
        this->EOffset->setText(QString::number(outgues[0],'f',4));
        this->EDamping->setText(QString::number(outgues[2],'f',4));
        this->EFreq->setText(QString::number(outgues[3],'f',4));
        this->EPhase->setText(QString::number(outgues[4],'f',4));
        this->EAmp->setText(QString::number(outgues[5],'f',4));
        this->fitPlot->setData(fitx,QVector<double>::fromStdVector(oscilations(outgues,fitx.toStdVector())));
        //this->plot->rescaleAxes();
        this->plot->replot();
    }
    else{
        inigues.resize(6);
        outgues.resize(6);
        inigues[0]=this->EOffset->text().toDouble();
        inigues[1]=this->ESlope->text().toDouble();
        inigues[2]=this->EDamping->text().toDouble();
        inigues[3]=this->EFreq->text().toDouble();
        inigues[4]=this->EPhase->text().toDouble();
        inigues[5]=this->EAmp->text().toDouble();
        model = fitter(fitx.toStdVector(),fity.toStdVector(),oscilations);
        outgues = optimizeM(model, inigues);
        this->ESlope->setText(QString::number(outgues[1],'f',4));
        this->EOffset->setText(QString::number(outgues[0],'f',4));
        this->EDamping->setText(QString::number(outgues[2],'f',4));
        this->EFreq->setText(QString::number(outgues[3],'f',4));
        this->EPhase->setText(QString::number(outgues[4],'f',4));
        this->EAmp->setText(QString::number(outgues[5],'f',4));
        this->fitPlot->setData(fitx,QVector<double>::fromStdVector(oscilations(outgues,fitx.toStdVector())));
        //this->plot->rescaleAxes();
        this->plot->replot();
    }
    emit frequenciEmit(outgues[3]);
}

QRheedAnal::~QRheedAnal()
{
        delete zoomIn;
}
