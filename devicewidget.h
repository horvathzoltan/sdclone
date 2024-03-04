#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>


class DeviceWidget: public QWidget
{
    Q_OBJECT

public:
    QString _usbDevicePath;
    QString _outputFileName;

    DeviceWidget();
};

#endif // DEVICEWIDGET_H
