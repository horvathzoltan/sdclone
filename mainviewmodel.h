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

    struct IntModel{
        int value;
    };

    struct StringListModel{
        QStringList txts;
    };

    struct DeviceModel{
        QString deviceLabel;
        QString usbDevicePath;
        QString devicePath;
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

    struct WriteStatus{
        QString devicePath;
        QString usbPath;
        bool status;
    };

    struct WriteStatusWM{
    private:
        QList<WriteStatus> _states;
    public:
        void append(const WriteStatus&m ){_states.append(m);}
        QList<WriteStatus> states()const{return _states;}
        void clear(){_states.clear();};
        bool isEmpty(){return _states.isEmpty();}
    };
};


#endif // MAINVIEWMODEL_H
