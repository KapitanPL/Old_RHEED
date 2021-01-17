#include "mainwindow.h"
#include <QTimer>
#include <QMessageBox>
#include <ctime>
#include <QColor>
#include <QLabel>
#include <QFileDialog>
#include <AnalyzeWindow/qrheedanal.h>
# include <fstream>
# include <iostream>
# include "myQtUtils.h"
#include <map>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QEventLoop>

#include "cameraselectwidget.h"


plotInfo::plotInfo(){}
plotInfo::~plotInfo(){
    delete this->Bdelete;
    delete this->CBColor;
    delete this->Lcolor;
    delete this->LFreq;
    delete this->rect;
    delete this->Banal;
}
void plotInfo::colorComboChanged(int sig){
    emit colorIndexChanged(sig);
}

void plotInfo::deleteClicked(){
    emit toBeDeleted();
}

void plotInfo::analClicked(){
    QCPDataMap *datas = (QCPDataMap*) this->graph->data();
    if (!datas->isEmpty()){
        QRheedAnal *eval = new QRheedAnal(datas);
        this->connect(eval,SIGNAL(frequenciEmit(double)),this,SLOT(frequenciGot(double)));
    }
}

void plotInfo::frequenciGot(double fr){
    this->LFreq->setText(QString::number(fr,'f',4));
    emit frequenciCheck();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    cam = NULL;
    this->chipW=656;
    this->chipH=494;
    this->buffLen=chipW*chipH;
    this->buffer=new double[buffLen];
    this->videoRunning=false;
    this->evaluationInProgress=false;
    this->setupUI(this);
    trigger = new QTimer();
    grafs = new QVector<plotInfo*>(0);
    colors = new QVector<SColors>(0);
    populateColors();
    this->videoFolder = "";
    this->histoGraph= this->plot->addGraph();
    this->histoGraph->setPen(QPen(Qt::black));
    load();
    initCamera();
}

void MainWindow::populateColors(){
    SColors clr;
    clr.name=QString("Red");
    clr.color=Qt::red;
    colors->append(clr);
    clr.name=QString("Green");
    clr.color=Qt::green;
    colors->append(clr);
    clr.name=QString("Blue");
    clr.color=Qt::blue;
    colors->append(clr);
    clr.name=QString("Magenta");
    clr.color=Qt::magenta;
    colors->append(clr);
    clr.name=QString("Dark Cyan");
    clr.color=Qt::darkCyan;
    colors->append(clr);
    clr.name=QString("Dark Magenta");
    clr.color=Qt::darkMagenta;
    colors->append(clr);
    clr.name=QString("Dark Blue");
    clr.color=Qt::darkBlue;
    colors->append(clr);
    clr.name=QString("Dark Gray");
    clr.color=Qt::darkGray;
    colors->append(clr);
}

void MainWindow::setupUI(QMainWindow *MainWindow){
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(600, 700);
    MainWindow->move(20,20);
    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    MainWindow->setCentralWidget(centralWidget);
    mainH = new QHBoxLayout(centralWidget);
    setupButtonGroup(mainH);
    imshow = new QtPlotCanvas(this->chipW,this->chipH,90);
    mainH->addWidget(imshow);
    plot = new QCustomPlot();
    plot->setFixedSize(500,300);
    plotingLayout = new QVBoxLayout();
    plotingLayout->addWidget(plot);
    setupButtonGroup2(plotingLayout);
    mainH->addLayout(plotingLayout);
    controlSpacer = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
    plotingLayout->addItem(controlSpacer);
}

void MainWindow::setupButtonGroup(QHBoxLayout * lay){
    QVBoxLayout * buttonLay = new QVBoxLayout();
    lay->addLayout(buttonLay);
    this->BInit = new QPushButton("Init Camera");
    this->BDispatch = new QPushButton("Dispatch Camera");
    this->BVideo = new QPushButton("Video");
    this->BLoadVideo = new QPushButton("Load Video");
    this->EIntTime = new QLineEdit("0.15");
    this->EWaitTime = new QLineEdit("0.1");
    this->recordVideo = new QCheckBox("Record video");
    this->BPath = new QPushButton("Video path");
    this->BPlayVideo = new QPushButton("Play Video");
    this->EIntegrate = new QLineEdit("1");
    this->BIntegrateImage = new QPushButton("Int. Image");
    this->BSaveImage = new QPushButton("Save Image");
    this->BHisto = new QPushButton("Histogram");
    buttonLay->addWidget(this->BInit);
    buttonLay->addWidget(this->BDispatch);
    buttonLay->addWidget(this->BVideo);
    buttonLay->addWidget(this->BLoadVideo);
    buttonLay->addWidget(this->BPlayVideo);
    this->BPlayVideo->setDisabled(true);
    buttonLay->addWidget(new QLabel("Integration time"));
    buttonLay->addWidget(this->EIntTime);
    buttonLay->addWidget(new QLabel("Pause time"));
    buttonLay->addWidget(this->EWaitTime);
    buttonLay->addWidget(this->recordVideo);
    buttonLay->addWidget(this->BPath);
    buttonLay->addWidget(this->EIntegrate);
    buttonLay->addWidget(this->BIntegrateImage);
    buttonLay->addWidget(this->BSaveImage);
    buttonLay->addStretch(1);
    buttonLay->addWidget(this->BHisto);
    this->BInit->connect(BInit,SIGNAL(clicked()),this,SLOT(initCamera()));
    this->BDispatch->connect(BDispatch,SIGNAL(clicked()),this,SLOT(dispatchCamera()));
    this->BVideo->connect(BVideo,SIGNAL(clicked()),this,SLOT(takeVideo()));
    this->BLoadVideo->connect(BLoadVideo,SIGNAL(clicked()),this,SLOT(loadVideoClicked()));
    this->BPlayVideo->connect(BPlayVideo,SIGNAL(clicked()),this,SLOT(playVideo()));
    this->EIntTime->connect(EIntTime,SIGNAL(editingFinished()),this,SLOT(changeExpositionTime()));
    this->EWaitTime->connect(EWaitTime,SIGNAL(editingFinished()),this,SLOT(changeExpositionTime()));
    this->recordVideo->connect(recordVideo,SIGNAL(stateChanged(int)),this,SLOT(recordVideoChanged(int)));
    this->BPath->connect(BPath,SIGNAL(clicked()),this,SLOT(chooseVideoPatrh()));
    this->BIntegrateImage->connect(BIntegrateImage,SIGNAL(clicked()),this,SLOT(integrateImage()));
    this->BSaveImage->connect(BSaveImage,SIGNAL(clicked()),this,SLOT(saveImage()));
    this->BHisto->connect(BHisto,SIGNAL(clicked()),this,SLOT(histogramChanged()));
    connect(this,SIGNAL(fullStop()),this,SLOT(emergencyStop()));
}

void MainWindow::setupButtonGroup2(QVBoxLayout *lay){
    QHBoxLayout * buttonLay = new QHBoxLayout();
    QHBoxLayout * controls = new QHBoxLayout();
    lay->addLayout(buttonLay);
    lay->addLayout(controls);
    this->BNew = new QPushButton("New");
    this->BEvaluate = new QPushButton("Evaluate");
    this->BClear = new QPushButton("Clear");
    this->BSave = new QPushButton("Save");
    this->BLoad = new QPushButton("Load");
    buttonLay->addWidget(this->BNew);
    buttonLay->addWidget(this->BEvaluate);
    buttonLay->addWidget(this->BClear);
    buttonLay->addWidget(this->BSave);
    buttonLay->addWidget(this->BLoad);
    connect(BNew,SIGNAL(clicked()),this,SLOT(newEval()));
    connect(BEvaluate,SIGNAL(clicked()),this,SLOT(evaluateChanged()));
    connect(BClear,SIGNAL(clicked()),this,SLOT(clearGraph()));
    connect(BSave,SIGNAL(clicked()),this,SLOT(saveData()));
    connect(BLoad,SIGNAL(clicked()),this,SLOT(loadData()));
    method = new QComboBox();
    method->addItem("Centroid_x");
    method->addItem("Centroid_y");
    method->addItem("Intensity");
    method->connect(method,SIGNAL(currentIndexChanged(int)),this,SLOT(methodChanged(int)));
    QLabel *lab = new QLabel("Method:");
    controls->addWidget(lab);
    controls->addWidget(this->method);
    controls->addStretch(1);
    this->Bzero = new QPushButton("Annulate");
    this->connect(this->Bzero,SIGNAL(clicked()),this,SLOT(annulateFreq()));
    controls->addWidget(this->Bzero);
    status = new QLabel();
    controls->addWidget(status);
    status->setText("0,0");
}

void MainWindow::annulateFreq(){
    this->status->setText("0.0");
    for (int i=0;i<this->grafs->size();++i){
        this->grafs->at(i)->LFreq->setText("0.0");
    }
}

void MainWindow::saveData(){
    if (!this->grafs->isEmpty()){
        QString saveFile = QFileDialog::getSaveFileName(this,"Save RHEED data:","",".txt");
        QByteArray byteArray=saveFile.toUtf8();
        const char *filename = byteArray.constData();
        std::ofstream sFile;
        char buffer[256];
        sFile.open(filename);
        int maxLen=0;
        int maxLenIndex=0;
        for (int j=0;j<this->grafs->size();++j){ //size checking
            if (this->grafs->at(j)->tim.size()>maxLen){
                maxLen=this->grafs->at(j)->tim.size();
                maxLenIndex=j;
            }
        }
        for (int i=0;i<maxLen;++i){
            sprintf_s(buffer,256,"%f",this->grafs->at(maxLenIndex)->tim.at(i));
            sFile << buffer;
            for (int j=0;j<grafs->size();j++){
                if (this->grafs->at(j)->intensity.size()>i){
                    sprintf_s(buffer,256," %f %f %f",this->grafs->at(j)->intensity.at(i),this->grafs->at(j)->centroid_x.at(i),this->grafs->at(j)->centroid_y.at(i));}
                else{
                    sprintf_s(buffer,256," NaN NaN NaN");
                }
                sFile << buffer;
            }
            sFile << "\n";
        }
        sFile.close();
    }
}

void MainWindow::loadData(){
    this->clearGraph();
    QString loadFile = QFileDialog::getOpenFileName(this,"Load RHEED data",this->videoFolder,"*.txt");
    QByteArray byteArray=loadFile.toUtf8();
    const char *filename = byteArray.constData();
    std::ifstream sFile;
    std::string line;
    sFile.open(filename);
    std::vector<double> row;
    std::vector< std::vector<double> > values;
    while (getline (sFile,line)){
        Str2vector(line,&row);
        if (values.empty()){
            for (unsigned i=0;i<row.size();++i){
                std::vector<double> tmp;
                tmp.push_back(row.at(i));
                values.push_back(tmp);
            }
        }
        for (unsigned i=0;i<row.size();++i){
            values.at(i).push_back(row.at(i));
        }
        row.clear();
    }
    sFile.close();
    if (!values.empty()){
        while (!this->grafs->empty()){
            this->grafs->at(0)->deleteClicked();
        }
        int grafNum = (values.size()-1)/2;
        for (int i=0;i<grafNum;++i){
            this->BNew->click();
            this->grafs->at(this->grafs->size()-1)->tim = QVector<double>::fromStdVector(values.at(0));
            this->grafs->at(this->grafs->size()-1)->intensity = QVector<double>::fromStdVector(values.at((i+1)*3-2));
            this->grafs->at(this->grafs->size()-1)->centroid_x = QVector<double>::fromStdVector(values.at((i+1)*3-1));
            this->grafs->at(this->grafs->size()-1)->centroid_y = QVector<double>::fromStdVector(values.at((i+1)*3));
        }
        this->method->setCurrentIndex(0);
        this->methodChanged(0);
    }
}

void MainWindow::evaluateChanged(){
    this->evaluationInProgress=!(this->evaluationInProgress);
    if (this->evaluationInProgress){
        if (this->histogram){
            this->histogram=false;
            this->BHisto->setStyleSheet("background-color:lightgray");
            this->histoGraph->clearData();
        }
        this->BEvaluate->setStyleSheet("background-color:lime");
    }
    else{
        this->BEvaluate->setStyleSheet("background-color:lightgray");
    }
}

void MainWindow::histogramChanged(){
    this->histogram=!(this->histogram);
    if (this->histogram){
        if (this->evaluationInProgress){
            this->evaluationInProgress=false;
            this->BEvaluate->setStyleSheet("background-color:lightgray");
        }
        this->BHisto->setStyleSheet("background-color:lime");
        this->clearGraph();
    }
    else{
        this->BHisto->setStyleSheet("background-color:lightgray");
        this->histoGraph->clearData();
    }
}

void MainWindow::newEval(){
    newEval(40, 20, 50, 50);
}

void MainWindow::newEval(qreal x, qreal y, qreal w, qreal h){
    plotInfo *info = new plotInfo() ;
    info->color = colors->at(grafs->size()%colors->size()).color;
    info->graph = this->plot->addGraph();
    info->graph->setPen(QPen(info->color));
    info->rect = new QChooseRect(0,0,w,h,this->imshow->pixmap,this->imshow->scene);
    info->rect->setPos(x,y);
    info->rect->setPen(QPen(info->color));
    info->connect(info,SIGNAL(frequenciCheck()),this,SLOT(recalculateFreq()));
    this->grafs->append(info);
    appendEvalGroup(plotingLayout,info);
    /*
    info.rect = new QGraphicsRectItem(20,20,100,100,this->imshow->pixmap,this->imshow->scene);
    info.rect->setPen(QPen(info.color));
    info.rect->setFlag(info.rect->ItemIsMovable, true);
    info.rect->setFlag(info.rect->ItemIsSelectable, true);
    */
}

void MainWindow::appendEvalGroup(QVBoxLayout* lay, plotInfo *info){
    lay->removeItem(controlSpacer);
    QHBoxLayout *tmp = new QHBoxLayout();
    lay->addLayout(tmp);
    info->Lcolor = new QLabel();
    QPixmap clr = QPixmap(60,20);
    clr.fill(info->color);
    info->Lcolor->setPixmap(clr);
    tmp->addWidget(info->Lcolor);
    info->CBColor = new QComboBox();
    for (int i=0; i<colors->size(); i++){
        info->CBColor->addItem(colors->at(i).name);}
    info->CBColor->setCurrentIndex((grafs->size()%colors->size())-1);
    info->CBColor->connect(info->CBColor,SIGNAL(currentIndexChanged(int)),info,SLOT(colorComboChanged(int)));
    info->connect(info,SIGNAL(colorIndexChanged(int)),this,SLOT(evalColorChanged(int)));
    tmp->addWidget(info->CBColor);
    info->Banal = new QPushButton("Analyze");
    info->Banal->connect(info->Banal,SIGNAL(clicked()),info,SLOT(analClicked()));
    tmp->addWidget(info->Banal);
    info->LFreq = new QLabel("0.0");
    tmp->addWidget(info->LFreq);
    info->CInclude = new QCheckBox("Include");
    //info->CInclude->setCheckState(Qt::Checked);
    info->CInclude->connect(info->CInclude,SIGNAL(stateChanged(int)),this,SLOT(recalculateFreq()));
    tmp->addWidget(info->CInclude);
    tmp->addStretch(1);
    info->Bdelete = new QPushButton("Delete");
    info->Bdelete->connect(info->Bdelete,SIGNAL(clicked()),info,SLOT(deleteClicked()));
    info->connect(info,SIGNAL(toBeDeleted()),this,SLOT(deletePlotGroup()));
    tmp->addWidget(info->Bdelete);
    lay->addItem(controlSpacer);
}

void MainWindow::evalColorChanged(int ind){
    QColor qclr = colors->at(ind).color;
    QPixmap clr = QPixmap(60,20);
    clr.fill(qclr);
    plotInfo * snd = dynamic_cast<plotInfo*>(sender());
    snd->Lcolor->setPixmap(clr);
    snd->color=qclr;
    snd->graph->setPen(QPen(qclr));
    snd->rect->setPen(qclr);
}

void MainWindow::deletePlotGroup(){
    plotInfo * snd = dynamic_cast<plotInfo*>(sender());
    for (int i =0; i<grafs->size(); i++){
        if (snd==grafs->at(i)){
            grafs->remove(i);
        }
    }
    this->plot->removeGraph(snd->graph);
    this->plot->replot();
    delete snd->rect;
    snd->rect = nullptr;
    delete snd->CInclude; //this one is not deleted as all others. Manual deletion neede. Don't know why.
    delete snd;
}

void MainWindow::clearGraph(){
    for (int i=0;i<grafs->size();i++){
        grafs->at(i)->graph->clearData();
        grafs->at(i)->tim.resize(0);
        grafs->at(i)->intensity.resize(0);
        grafs->at(i)->centroid_x.resize(0);
        grafs->at(i)->centroid_y.resize(0);
    }
    this->plot->replot();
}

void MainWindow::recalculateFreq(){
    double freq = 0.0;
    int nonZero = 0;
    double val=0.0;
    double deviation=0.0;
    for (int i = 0; i<this->grafs->size(); ++i){
        val = this->grafs->at(i)->LFreq->text().toDouble();
        if (val!=0.0 && this->grafs->at(i)->CInclude->isChecked()){
            freq+=val;
            ++nonZero;
        }
    }
    if (nonZero!=0){
        freq /= nonZero;
    }
    nonZero=0;
    for (int i = 0; i<this->grafs->size(); ++i){
        val = this->grafs->at(i)->LFreq->text().toDouble();
        if (val!=0.0 && this->grafs->at(i)->CInclude->isChecked()){
            deviation+=(val-freq)*(val-freq);
            ++nonZero;
        }
    }
    if (nonZero!=0){
        deviation /= nonZero;
    }
    this->status->setText(QString::number(freq,'f',4)+"+-"+QString::number(deviation,'f',4));
}

void MainWindow::initCamera(){
    if (cam==NULL){
    try {
        cam= new TCamera();
        cam->initCamera(this->EIntTime->text().toDouble());
        connect(this->cam, SIGNAL(pictureReady()), this, SLOT(plotImage()));
    }
    catch (const char* msg){
        QMessageBox::information(this,"Rheed2.0 Error",msg);
        cam=NULL;
    }
    }
    else
        QMessageBox::information(this,"Rheed2.0 Error","Camera running");
}

void MainWindow::dispatchCamera(){
    if (cam!=NULL){
    try {
        cam->dispatchCamera();
        cam->~TCamera();
        cam=NULL;
    }
    catch (const char* msg){
        QMessageBox::information(this,"Rheed2.0 Error",msg);
    }
    }
}

void MainWindow::plotImage(){
    if (this->integrationInProgress){
        if (this->intIterator < this->EIntegrate->text().toInt()){
            for (unsigned i=0;i<this->buffLen;i++){
                this->buffer[i]=(this->buffer[i]*this->intIterator+this->cam->doubleBuffer[i])/(this->intIterator+1);
            }
            ++this->intIterator;
        }
        else{
            this->trigger->stop();
            this->BVideo->setEnabled(true);
            this->trigger->disconnect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
            this->integrationInProgress = false;
        }
    }
    else{
        double mean = this->cam->doubleBuffer[0];
        double tim = *this->cam->ptime;
        for (unsigned i=0;i<this->buffLen;i++){
            this->buffer[i]=this->cam->doubleBuffer[i];
            if (i>1){
                mean=((i-1)*mean+this->buffer[i])/i;}
        }
        if (this->evaluationInProgress){
            for (int i=0;i<grafs->size();i++){
                if (this->grafs->at(i)->tim.isEmpty()){
                    this->grafs->at(i)->tim.push_back(tim);
                }
                else{
                    this->grafs->at(i)->tim.push_back(tim+this->grafs->at(i)->tim.last());
                }
                evaluatePoints(i);
                switch (this->method->currentIndex())
                {
                case 0:
                    this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_x);
                    break;
                case 1:
                    this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_y);
                    break;
                case 2:
                    this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
                    break;
                default:
                    this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
                    break;
                }

            }
            this->plot->rescaleAxes(true);
            this->plot->replot();
        }
        if (this->histogram){
            QVector<double> inten, counts;
            for (this->cam->ito = this->cam->histo.begin(); this->cam->ito != this->cam->histo.end(); ++this->cam->ito){
                inten.push_back(double(this->cam->ito->first));
                counts.push_back(double(this->cam->ito->second));
            }
            this->histoGraph->setData(inten,counts);
            this->plot->rescaleAxes(true);
            this->plot->replot();
        }
    }
    this->imshow->plotMatrix(this->buffer,this->chipW,this->chipH);
}

void MainWindow::plotVideoImage(double tim){ //this->buffer must be filled with loaded image
    if (this->evaluationInProgress){
        for (int i=0;i<grafs->size();i++){
            if (this->grafs->at(i)->tim.isEmpty()){
                this->grafs->at(i)->tim.push_back(tim);
            }
            else{
                this->grafs->at(i)->tim.push_back(tim+this->grafs->at(i)->tim.last());
            }
            evaluatePoints(i);
            switch (this->method->currentIndex())
            {
            case 0:
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_x);
                break;
            case 1:
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_y);
                break;
            case 2:
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
                break;
            default:
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
                break;
            }

        }
        this->plot->rescaleAxes(true);
        this->plot->replot();
    }
    checkBuffer(buffer,buffLen);
    this->imshow->plotMatrix(this->buffer,this->chipW,this->chipH);
    QTime dieTime= QTime::currentTime().addMSecs(50);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    this->imshow->update();
    if (this->loadedVideoRunning){emit nextFramePleas();}
}

void MainWindow::saveImage(){
    QString saveFile = QFileDialog::getSaveFileName(this,"Save RHEED image:","",".png");
    this->imshow->canvas->save(saveFile);
}

void MainWindow::methodChanged(int method){
    if (!this->loadedVideoRunning || (this->loadedVideoRunning && !this->evaluationInProgress)){
        switch (method)
        {
        case 0:
        {
            for (int i=0;i<this->grafs->size();++i){
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_x);
            }
            break;
        }
        case 1:
            {
            for (int i=0;i<this->grafs->size();++i){
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->centroid_y);
            }
            break;
            }
        case 2:
            for (int i=0;i<this->grafs->size();++i){
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
            }
            break;
        default:
            for (int i=0;i<this->grafs->size();++i){
                this->grafs->at(i)->graph->setData(this->grafs->at(i)->tim,this->grafs->at(i)->intensity);
            }
            break;
        }
    this->plot->rescaleAxes(true);
    this->plot->replot();
    }
}

double MainWindow::evaluatePoints(int index){
    QRectF area = this->grafs->at(index)->rect->boundingRect();
    QPointF ar = this->grafs->at(index)->rect->pos();
    int x =(int) ar.x()+area.topLeft().x();
    int y =(int) ar.y()+area.topLeft().y();
    int w =(int) area.width();
    int h =(int) area.height();
    int chw =(int) this->imshow->width;
    double inten = 0.0;
    double ex=0.0;
    double yp=0.0;
    if (x<0){x=0;}
    if (y<0){y=0;}
    for (unsigned i=0; i<unsigned(w*h);i++){
        inten+=this->buffer[(y+i/w)*chw+x+i%w];
        ex+=this->buffer[(y+i/w)*chw+x+i%w]*(i%w);
        yp+=this->buffer[(y+i/w)*chw+x+i%w]*(i/w);
    }
    ex/=inten;
    yp/=inten;
    this->grafs->at(index)->centroid_x.push_back(ex);
    this->grafs->at(index)->centroid_y.push_back(yp);
    this->grafs->at(index)->intensity.push_back(inten);
    return inten;
}

void MainWindow::save()
{
    QString name = QString("guiData");
    QFile fGuiData(QString("%1.json").arg(name));
    if (fGuiData.open(QIODevice::WriteOnly))
    {
        QJsonObject saveObject;
        //save here

        QJsonValue sVideoPath = QJsonValue(this->videoFolder);
        saveObject["sVideoPath"] = sVideoPath;

        QJsonArray listROIs;
        if (this->grafs)
        {
            for (auto itGraf : (*this->grafs))
            {
                QJsonObject iRoi;
                iRoi["x"] = QJsonValue(itGraf->rect->pos().x());
                iRoi["y"] = QJsonValue(itGraf->rect->pos().y());
                iRoi["w"] = QJsonValue(itGraf->rect->rect().width());
                iRoi["h"] = QJsonValue(itGraf->rect->rect().height());
                listROIs.append(iRoi);
            }
            saveObject["listROIs"] = listROIs;
        }

        QJsonValue integrationTime = QJsonValue(this->EIntTime->text().toDouble());
        saveObject["integrationTime"] = integrationTime;

        QJsonValue pauseTime = QJsonValue(this->EWaitTime->text().toDouble());
        saveObject["waitTime"] = pauseTime;

        QJsonDocument saveDoc(saveObject);
        fGuiData.write(saveDoc.toJson());

        fGuiData.close();
    }
}

void MainWindow::load()
{
    QString name = QString("guiData");
    if (QFile::exists(QString("%1.json").arg(name)))
    {
        QFile fGuiData(QString("%1.json").arg(name));
        if (fGuiData.open(QIODevice::ReadOnly))
        {
            QByteArray saveData = fGuiData.readAll();

            QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

            QJsonObject loadVariant = loadDoc.object();
            if (loadVariant.contains("sVideoPath") && loadVariant["sVideoPath"].isString())
            {
                this->videoFolder = loadVariant["sVideoPath"].toString();
            }
            if (loadVariant.contains("listROIs") && loadVariant["listROIs"].isArray())
            {
                QJsonArray entries = loadVariant["listROIs"].toArray();
                for (auto entry : entries)
                {
                    QJsonObject iRoi = entry.toObject();
                    qreal x = iRoi["x"].toDouble();
                    qreal y = iRoi["y"].toDouble();
                    qreal w = iRoi["w"].toDouble();
                    qreal h = iRoi["h"].toDouble();
                    newEval(x, y, w, h);
                }
            }
            if (loadVariant.contains("integrationTime") && loadVariant["integrationTime"].isDouble())
                this->EIntTime->setText(QString("%1").arg(loadVariant["integrationTime"].toDouble()));
            if (loadVariant.contains("waitTime") && loadVariant["waitTime"].isDouble())
                this->EWaitTime->setText(QString("%1").arg(loadVariant["waitTime"].toDouble()));
            fGuiData.close();
        }
    }
}

void MainWindow::emergencyStop(){
    this->trigger->stop();
    this->BVideo->setText("Video");
    this->trigger->disconnect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
}

void MainWindow::takePicture(){
    if (cam!=NULL){
        QTimer::singleShot(0,this->cam, SLOT(takePicture()));
    }//if cam
    else
        QMessageBox::information(this,"Rheed2.0 Error","Camera not initialised properly");
}

void MainWindow::loadVideoClicked(){
    QString videoFolder = QFileDialog::getExistingDirectory(this,"Load RHEED video:",this->videoFolder);
    if (videoFolder.length()==0) {return;}
    QDir vFolder = QDir(videoFolder);
    QStringList files = vFolder.entryList(QStringList("*.rid"),QDir::Files,QDir::Name);
    QFile timefile(videoFolder+"/time.txt");
    FILE *infile = NULL;
    unsigned short *tmp = new unsigned short[this->buffLen];
    if (!timefile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this,"Rheed2.0 Error","No timefile in folder");
        return;
    }
    QVector<double> times;
    while (!timefile.atEnd()){
        times.append(QString(timefile.readLine()).toDouble());
    }
    timefile.close();
    if (times.size()==files.size()){
        QString filename = videoFolder+"/"+files.at(0);
        //qDebug() << filename;
        infile = fopen(filename.toLocal8Bit().data(),"r");
        if (!infile) return;
        unsigned int uiRead = fread(tmp,sizeof(unsigned short),this->buffLen,infile);
        //qDebug() << "LoadVieo Read: " << uiRead;
        loadedTimes=times;
        videoFiles=files;
        this->videoFolder=videoFolder;
        for (unsigned j=0;j<this->buffLen;j++){
            this->buffer[j]=(double)tmp[j];
        }
        checkBuffer(buffer,buffLen);
        fclose(infile);
        //stop video if running
        if (this->BVideo->text()=="STOP"){this->emergencyStop();}
        this->imshow->plotMatrix(this->buffer,this->chipW,this->chipH);
        this->BPlayVideo->setEnabled(true);
        this->vidIterator = 0;
        this->connect(this,SIGNAL(nextFramePleas()),this,SLOT(playVideo()));
    }
    else{
        QMessageBox::information(this,"Rheed2.0 Error","Time file size doesn't match .rid files");
        return;
    }
    delete[] tmp;
}

void MainWindow::playVideo(){
    FILE *infile = NULL;
    int i = this->vidIterator;
    this->loadedVideoRunning = true;
    if (i == 0)
    {
        this->imshow->iCounter = 0;
    }
    if (bVideoFinished)
    {
        clearGraph();
        bVideoFinished = false;
    }
    if (i<this->videoFiles.size()){
        //qDebug() << i << this->videoFiles.at(i);
        this->vidIterator++;
        QString filename = videoFolder+"/"+this->videoFiles.at(i);
        //qDebug() << filename;
        infile = fopen(filename.toLocal8Bit().data(),"r");;
        if (!infile) return;
        unsigned short *tmp = new unsigned short[this->buffLen];
        unsigned int uiRead = fread(tmp,sizeof(unsigned short),this->buffLen,infile);
        //qDebug() << uiRead << ", " << buffLen;
        for (unsigned j=0;j<this->buffLen;j++){
            this->buffer[j]=(double)tmp[j];
            //qDebug()<< j<< ", " << tmp[j] << ", " << this->buffer[j];
        }
        checkBuffer(buffer, buffLen);
        fclose(infile);
        this->plotVideoImage(this->loadedTimes.at(i));
        delete[] tmp;
    }
    else{
        this->vidIterator = 0;
        this->disconnect(this,SIGNAL(nextFramePleas()),this,SLOT(playVideo()));
        this->loadedVideoRunning = false;
        bVideoFinished = true;
    }
}

void MainWindow::takeVideo(){
    if (cam!=NULL){
        if (this->BVideo->text()=="Video"){
            connect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
            int inte = int((this->EIntTime->text().toDouble()+this->EWaitTime->text().toDouble())*1000+0.5);
            this->trigger->start(inte);
            this->BVideo->setText("STOP");
        } //if Video text
        else{
            this->trigger->stop();
            this->BVideo->setText("Video");
            this->trigger->disconnect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
        } //else Video text
    }//if cam
    else
        QMessageBox::information(this,"Rheed2.0 Error","Camera not initialised properly");
}

void MainWindow::changeExpositionTime(){
    if (cam!=NULL){
        cam->expositionTime=this->EIntTime->text().toDouble();
        this->trigger->setInterval(int((this->EIntTime->text().toDouble()+this->EWaitTime->text().toDouble())*1000+0.5));
    }
}

void MainWindow::recordVideoChanged(int state){
    this->cam->video(state);
}

void MainWindow::chooseVideoPatrh(){
    QString path;
    path = QFileDialog::getExistingDirectory(this,"Save directory");
    QByteArray ba = path.toLocal8Bit();
    char *c_str2 = ba.data();
    this->cam->cam->setVideoPath(c_str2);
}

void MainWindow::integrateImage(){
    if (cam!=NULL){
        if (this->BVideo->text()=="STOP"){ //STOP video if running
            this->trigger->stop();
            this->BVideo->setText("Video");
            this->trigger->disconnect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
        }
        this->integrationInProgress = true;
        this->BVideo->setDisabled(true);
        connect(this->trigger,SIGNAL(timeout()),this->cam,SLOT(takePicture()));
        int inte = int((this->EIntTime->text().toDouble()+this->EWaitTime->text().toDouble())*1000+0.5);
        intIterator=0;
        for (unsigned i=0;i<this->buffLen;i++){
            this->buffer[i]=0;}
        this->trigger->start(inte);
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "RHEED2.0", tr("Are you sure?\n"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        QTimer::singleShot(0,this,SLOT(emergencyStop()));
        QTimer::singleShot(0,this,SLOT(dispatchCamera()));
        event->accept();
    } else {
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    save();
    delete trigger;
}

TCamera::TCamera(unsigned buffLen): bufflen(buffLen){
    this->cam=NULL;
}

TCamera::~TCamera(){
}

void TCamera::initCamera(double eTime){
    if (this->cam==NULL){
        try {
            this->cam= new camera(
                        [](std::vector<unsigned>& IDs)->unsigned
                        {
                            unsigned uiRet = 0;
                            CameraSelectWidget selectDialog(IDs, &uiRet);
                            selectDialog.exec();
                            return uiRet;
                        }

                    );
        } catch (const char* msg){
            this->cam=NULL;
            throw(msg);
        }
        catch (std::exception &ex){
            this->cam=NULL;
            throw(ex);
        }
        this->expositionTime=eTime;
        this->buffer = new unsigned short[bufflen];
        doubleBuffer=new double[bufflen];
        writeDoubleBuffer=new double[bufflen];
        ptime = new double;
        *ptime = -1.0;
        readLock=false;
    }
    else
        QMessageBox::information(0,"Rheed2.0 Error","TCamera running");
}

void TCamera::dispatchCamera(){
    if (this->cam!=NULL){
    try {
        delete this->cam;
        this->cam=NULL;
    }
    catch (const char* msg){
        QMessageBox::information(0,"Rheed2.0 Error",msg);
    }
    }
    else
        QMessageBox::information(0,"Rheed2.0 Error","Camera was not running");
}

void TCamera::takePicture(){
    if (this->cam!=NULL){
        this->cam->takeFrame(this->expositionTime,this->buffer,this->bufflen,this->ptime);
        histo.clear();
        for (unsigned i=0; i<bufflen; i++){
            this->writeDoubleBuffer[i]=double(this->buffer[i]);
            ito = histo.find(this->buffer[i]);
            if (ito==histo.end()){
                histo[this->buffer[i]]=1;
            }
            else{
                ++histo[this->buffer[i]];
            }
        }
        doubleBuffer = writeDoubleBuffer;
        emit pictureReady();
    }//if cam
    else
        QMessageBox::information(0,"Rheed2.0 Error","Camera not initialised properly");
}

void TCamera::changeExpositionTime(double time){
    this->expositionTime=time;
}

void TCamera::video(int st){
    this->cam->videoOnOff(st);
    if (st){
        //this->cam->setVideoPath();
        this->cam->resetVideoCounters();
    }
}

unsigned TCamera::selectCamera(std::vector<unsigned> &vector)
{
    unsigned ret = 0;
    if ( vector.size() )
    {
        if (vector.size() == 1)
            ret = vector[0];
        else
        {

        }
    }
    return ret;
}
