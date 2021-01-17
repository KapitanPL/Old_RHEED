#-------------------------------------------------
#
# Project created by QtCreator 2014-02-12T10:27:48
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Rheed2
TEMPLATE = app


SOURCES += main.cpp\
    cameraselectwidget.cpp \
        mainwindow.cpp \
    QtCanvas/qtplotcanvas.cpp \
    QtCanvas/qtcolorbar.cpp \
    QCustomPlot/qcustomplot.cpp \
    camera/camera.cpp \
    QChooseRect/qchooserect.cpp \
    AnalyzeWindow/qrheedanal.cpp \
    AnalyzeWindow/fitter/fitter.cpp \
    myQtUtils.cpp

HEADERS  += mainwindow.h \
    QtCanvas/qtplotcanvas.h \
    QCustomPlot/qcustomplot.h \
    camera/camera.h \
    QtCanvas/qtcolorbar.h \
    QCustomPlot/qcustomplot.h \
    QtCanvas/qtcolorbar.h \
    QChooseRect/qchooserect.h \
    AnalyzeWindow/qrheedanal.h \
    AnalyzeWindow/fitter/fitter.h \
    AnalyzeWindow/fitter/simplex.h \
    cameraselectwidget.h \
    myQtUtils.h
