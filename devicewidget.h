#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QLabel>
#include <QWidget>


class DeviceWidget: public QWidget
{
    Q_OBJECT

private:
    QLabel* _statusLabel;
public:
    QString _usbDevicePath;
    QString _outputFileName;
    QString _serial;

    DeviceWidget();

    void setStatus(bool status);
    void setStatusLabel(QLabel*v){_statusLabel=v;}
};

#endif // DEVICEWIDGET_H
