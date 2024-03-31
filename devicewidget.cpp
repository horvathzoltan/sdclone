#include "devicewidget.h"

DeviceWidget::DeviceWidget() {}

void DeviceWidget::setStatus(bool status)
{
    QString msg = status?"OK":"FAILED";
    _statusLabel->setText(msg);
}
