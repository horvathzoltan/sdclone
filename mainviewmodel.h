#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QtGlobal>
#include <QList>
#include <QColor>

class MainViewModel
{
public:

    struct DoWorkModel{
        int i;
    };

    struct DoWorkRModel{
        QString txt;
    };

    struct StringModel{
        QString txt;
    };

    struct StringListModel{
        QStringList txts;
    };

    struct DeviceModel{
        QString deviceLabel;
        QString usbDevicePath;
        QString serial;
        QString outputFileName;
        QStringList partitionLabels;
    };

    struct DeviceListModel{
        QList<DeviceModel> devices;

        bool containsBySerial(const QString& s) const{
            for(auto&a:devices){
                if(a.serial==s) return true;
            }
            return false;
        }
    };
};


#endif // MAINVIEWMODEL_H
