#include "devicewidget.h"

DeviceWidget::DeviceWidget() {}

void DeviceWidget::setStatus(bool status)
{
    //QString msg = status?"OK":"FAILED";
    QColor color = status?Qt::green:Qt::red;

    QPalette pal = QPalette();
    pal.setColor(_statusLabel->backgroundRole(), color);
    //_statusLabel->setText(msg);
    _statusLabel->setPalette(pal);
    _statusLabel->setAutoFillBackground(true);
    _statusLabel->update();

    //pal = QPalette();
    pal.setColor(_labelLabel->backgroundRole(), color);
    _labelLabel->setPalette(pal);
    _labelLabel->setAutoFillBackground(true);
    _labelLabel->update();
}

void DeviceWidget::resetStatus()
{
    //QString msg = status?"OK":"FAILED";
    //QColor color = status?Qt::green:Qt::red;

    QPalette pal = QPalette();
    pal.color(_statusLabel->backgroundRole());
    pal.setColor(_statusLabel->backgroundRole(), _defaultBackground);
    //_statusLabel->setText(msg);
    _statusLabel->setPalette(pal);
    _statusLabel->setAutoFillBackground(true);
    _statusLabel->update();

    //pal = QPalette();
    pal.setColor(_labelLabel->backgroundRole(), _defaultBackground);
    _labelLabel->setPalette(pal);
    _labelLabel->setAutoFillBackground(true);
    _labelLabel->update();
}
