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
    QLabel* _l2;
    QColor _defaultBackground;
public:
    QString _usbDevicePath;
    QString _outputFileName;
    QString _serial;
    QString _devicePath;

    DeviceWidget();

    void setStatus(bool status);
    void resetStatus();
    void setStatusLabel(QLabel*v){_statusLabel=v;}
    void setLabelLabel(QLabel*v){_labelLabel=v;}
    void setL2(QLabel*v){_l2=v;}
    //QLabel* getL2(){return _l2;}
    void setDefaultBackground(QColor v){_defaultBackground = v;}
    void UpdateL2(const QString& txt);
    int GetPt(const QString& txt);
};

#endif // DEVICEWIDGET_H
