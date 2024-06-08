#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include <QFileInfo>
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

        bool isValid() {return !usbPath.isEmpty();}
    };
private:
    QList<DeviceModel> _devices;
    ProcessHelper _pollingProcessHelper;
    quint32 _pollingCounter = 0;

    bool _isInPolling=false;
    QString _usbRootPath;
    //QString _usbRoot;
    void GetPartitionData(DeviceModel *);
    QString GetUsbPath(const QString& str);

    int indexOf(const QString& devPath);
public:
    void setUsbRootPath(const QString& v){ _usbRootPath = v;}
    QString usbRootPath(){return _usbRootPath;}

    DeviceStorage::DeviceModel Add(const QString& devPath);
    void Remove(const QString& devPath);
    DeviceStorage::DeviceModel Update(const QString& devPath);

    QList<DeviceModel> devices(){return _devices;}
    //DeviceStorage();
    void Init();
    void Init2();
    DeviceModel CreateDevice(const QFileInfo& devPath);
    DeviceModel CreateDevice_BySys(const QFileInfo& sysPath);
    QFileInfo FindByPath(const QString& devPath );

    bool IsInPolling(){return _isInPolling;}

    int GetSize(const QString &d);
signals:
    void initFinished();

private slots:
    //void stdErrReader(QByteArray&d);
    void finished();
};

#endif // DEVICESTORAGE_H
