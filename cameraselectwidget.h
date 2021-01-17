#ifndef CAMERASELECTWIDGET_H
#define CAMERASELECTWIDGET_H

#include <QDialog>

class CameraSelectWidget : public QDialog
{
    Q_OBJECT
public:
    CameraSelectWidget(std::vector<unsigned> & cameraIDs, unsigned * IDout, QWidget *parent = nullptr);
private:
    unsigned * m_idOut = nullptr;

private slots:
    void comboChanged(int iIndex);
};


#endif // CAMERASELECTWIDGET_H
