#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCanvas/qtplotcanvas.h>
#include <QCustomPlot/qcustomplot.h>
#include <camera/camera.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QChooseRect/qchooserect.h>

class plotInfo: public QObject{
    Q_OBJECT
public:
    plotInfo();
    ~plotInfo();
    QVector<double> tim;
    QVector<double> centroid_x;
    QVector<double> centroid_y;
    QVector<double> intensity;
    QCPGraph *graph;
    QChooseRect * rect;
    QColor color;
    QLabel *Lcolor, *LFreq;
    QComboBox *CBColor;
    QPushButton *Banal,*Bdelete;
    QCheckBox *CInclude;
signals:
    void colorIndexChanged(int);
    void toBeDeleted();
    void frequenciCheck();
public slots:
    void frequenciGot(double);
    void deleteClicked();
    void colorComboChanged(int);
    void analClicked();

};

struct SColors{
    QString name;
    QColor color;
};

class TCamera: public QObject {
    Q_OBJECT
public:
    camera * cam;
    TCamera(unsigned buffLen=648128);
    ~TCamera();
    double * doubleBuffer;
    double *ptime;
    bool readLock;
    double expositionTime;
    void initCamera(double eTime = 0.15);
    void dispatchCamera();
    void changeExpositionTime(double time);
    void video(int st);
    std::map<unsigned short, int> histo;
    std::map<unsigned short, int>::iterator ito;
    static unsigned selectCamera(std::vector<unsigned> & vector);
public slots:
    void takePicture();
signals:
    void pictureReady();
private:
    double * writeDoubleBuffer;
    unsigned short * buffer;
    unsigned bufflen;
    int chipW,chipH;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void initCamera();
    void dispatchCamera();
    void takePicture();
    void takeVideo();
    void plotImage();
    void plotVideoImage(double tim);
    void changeExpositionTime();
    void emergencyStop();
    void newEval();
    void newEval(qreal x, qreal y, qreal w, qreal h);
    void evalColorChanged(int ind);
    void deletePlotGroup();
    void evaluateChanged();
    void clearGraph();
    void recordVideoChanged(int state);
    void chooseVideoPatrh();
    void saveData();
    void loadData();
    void methodChanged(int);
    void loadVideoClicked();
    void playVideo();
    void recalculateFreq();
    void saveImage();
    void integrateImage();
    void annulateFreq();
    void histogramChanged();
signals:
    void pictureLoaded(double);
    void fullStop();
    void nextFramePleas();
private:
    QLabel *status;
    QPushButton *BInit,*BDispatch,*BVideo, *BLoadVideo,*BTest, *BNew, *BEvaluate, *BClear, *BSave, *BLoad, *BPath, *BPlayVideo, *BIntegrateImage, *BSaveImage, *Bzero, *BHisto;
    QLineEdit *EIntTime, *EWaitTime, *EIntegrate;
    QWidget* centralWidget;
    TCamera * cam;
    QtPlotCanvas * imshow;
    QCustomPlot * plot;
    QHBoxLayout * mainH;
    QTimer *trigger;
    QVector<SColors> *colors;
    QVector<plotInfo*> *grafs;
    QVBoxLayout *plotingLayout;
    QVector<double> loadedTimes;
    QStringList videoFiles;
    QString videoFolder;
    QComboBox *method;
    QSpacerItem * controlSpacer;
    QCheckBox * recordVideo;
    QCPGraph *histoGraph;
    double tmin,tmax,pmin,pmax;
    void closeEvent(QCloseEvent *event);
    void setupUI(QMainWindow *MainWindow);
    void appendEvalGroup(QVBoxLayout* lay, plotInfo *info);
    void setupButtonGroup(QHBoxLayout * lay);
    void setupButtonGroup2(QVBoxLayout * lay);
    void populateColors();
    double evaluatePoints(int index);
    double *buffer = nullptr;
    unsigned buffLen;
    int chipW, chipH;
    int vidIterator, intIterator;
    char msg[256];
    bool videoRunning, evaluationInProgress, loadedVideoRunning, integrationInProgress, histogram;
    bool bVideoFinished = false;
    //double expositionTime;

    void save();
    void load();
};

#endif // MAINWINDOW_H

