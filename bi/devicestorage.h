#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include <QList>
#include <QString>

class DeviceStorage
{
public:
    struct PartitionModel{
        QString partPath;
        QString label;
        QString project;
        QString bin;

        static PartitionModel Parse(const QString& txt);

        QString toString() const;
    };
    struct DeviceModel{
        QString devPath;
        QString usbPath;
        QString serial;

        QList<PartitionModel> partitions;

        static DeviceModel Parse(const QString& txt);
        
        QString toString();
        QString usbRootPath();
    };
private:
    QList<DeviceModel> _devices;

public:
    QList<DeviceModel> devices(){return _devices;}
    DeviceStorage();
    void Init();
    QString usbRootPath();
};

#endif // DEVICESTORAGE_H
