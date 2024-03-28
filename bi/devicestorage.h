#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include <QList>
#include <QString>

#include <helpers/processhelper.h>

class DeviceStorage : public QObject
{
    Q_OBJECT

public:
    explicit DeviceStorage(QObject *parent = nullptr);

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
    ProcessHelper _pollingProcessHelper;
    quint32 _pollingCounter = 0;

    bool _isInPolling=false;
    QString _usbRootPath;

public:
    QList<DeviceModel> devices(){return _devices;}
    //DeviceStorage();
    void Init();
    QString usbRootPath();
    bool IsInPolling(){return _isInPolling;}

signals:
    void initFinished();

private slots:
    //void stdErrReader(QByteArray&d);
    void finished();
};

#endif // DEVICESTORAGE_H
