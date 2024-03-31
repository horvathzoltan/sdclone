#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QLabel>
#include <QWidget>


class DeviceWidget: public QWidget
{
    Q_OBJECT

private:
    QLabel* _statusLabel;
    QLabel* _labelLabel;
    QColor _defaultBackground;
public:
    QString _usbDevicePath;
    QString _outputFileName;
    QString _serial;

    DeviceWidget();

    void setStatus(bool status);
    void resetStatus();
    void setStatusLabel(QLabel*v){_statusLabel=v;}
    void setLabelLabel(QLabel*v){_labelLabel=v;}
    void setDefaultBackground(QColor v){_defaultBackground = v;}
};

#endif // DEVICEWIDGET_H
