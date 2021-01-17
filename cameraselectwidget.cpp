#include "cameraselectwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CameraSelectWidget::CameraSelectWidget(std::vector<unsigned> &cameraIDs, unsigned * IDout, QWidget *parent)
    : QDialog(parent),
      m_idOut(IDout)
{
    if (cameraIDs.size() && m_idOut)
        *m_idOut = cameraIDs[0];
    QVBoxLayout * pMainLayout = new QVBoxLayout();
    setLayout(pMainLayout);

    QLabel * info = new QLabel("Select your camera and press OK.");
    pMainLayout->addWidget(info);

    QHBoxLayout * comboLayout = new QHBoxLayout();
    comboLayout->addStretch(1);
    QComboBox * combo = new QComboBox();
    connect(combo, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &CameraSelectWidget::comboChanged);
    comboLayout->addWidget(combo);
    comboLayout->addStretch(1);
    pMainLayout->addLayout(comboLayout);

    for (auto it : cameraIDs)
        combo->addItem(QString("%1").arg(it));

    QHBoxLayout * okLayout = new QHBoxLayout();
    okLayout->addStretch(1);
    QPushButton * pushOk = new QPushButton("Ok");
    connect(pushOk, &QPushButton::clicked, this, &CameraSelectWidget::accept);
    okLayout->addWidget(pushOk);
    pMainLayout->addLayout(okLayout);
}

void CameraSelectWidget::comboChanged(int /*iIndex*/)
{
    QComboBox * combo = static_cast<QComboBox*>(sender());
    if (combo && m_idOut)
    {
        *m_idOut = combo->currentText().toUInt();
    }

}
