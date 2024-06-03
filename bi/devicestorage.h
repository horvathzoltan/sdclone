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
        //QString serial;
        quint64 size;

        QList<PartitionModel> partitions;

        static DeviceModel Parse(const QString& txt);
        
        QString toString();
        QString usbRootPath();

        QString serial() const;
    };
private:
    QList<DeviceModel> _devices;
    ProcessHelper _pollingProcessHelper;
    quint32 _pollingCounter = 0;

    bool _isInPolling=false;
    QString _usbRootPath;

    void GetPartitionData(DeviceModel *);


    int indexOf(const QString& devPath);
public:
    void Remove(const QString& devPath);
    QList<DeviceModel> devices(){return _devices;}
    //DeviceStorage();
    void Init();
    void Init2();
    QString usbRootPath();
    bool IsInPolling(){return _isInPolling;}



    int GetSize(const QString &d);
signals:
    void initFinished();

private slots:
    //void stdErrReader(QByteArray&d);
    void finished();
};

#endif // DEVICESTORAGE_H
