#include "devicestorage.h"

#include <helpers/processhelper.h>

#include <QElapsedTimer>

extern ProcessHelper _processHelper;


DeviceStorage::DeviceStorage(QObject *parent) :QObject(parent)
{
    _pollingProcessHelper.SetPassword("Aladar123");
    QObject::connect(&_pollingProcessHelper, &ProcessHelper::finished,
                     this, &DeviceStorage::finished);
}

// DeviceStorage::DeviceStorage() {
//     _pollingProcessHelper.SetPassword("Aladar123");
//     QObject::connect(&_pollingProcessHelper, &ProcessHelper::finished,
//                      this, &DeviceStorage::finished);
// }

void DeviceStorage::Init()
{
    _isInPolling = true;
    _pollingCounter++;

    QString cmd= QStringLiteral("/home/pi/readsd/bin/readsd -q -s Aladar123");
    _pollingProcessHelper.ShellExecuteNoWait(cmd);
}

void DeviceStorage::finished()
{
    _devices.clear();
    _usbRootPath = "";

    ProcessHelper::Output out = _pollingProcessHelper.GetOut();
    bool valid = out.exitCode==0 && !out.stdOut.isEmpty();
    if(valid) {
        QStringList lines = out.stdOut.split('\n');
        for (QString &l : lines) {
            if (l.isEmpty()) continue;

            DeviceModel d = DeviceModel::Parse(l);
            if (!d.devPath.isEmpty()) {
                _devices.append(d);
            }
        }

        if(!_devices.isEmpty()){
            DeviceModel device = _devices.first();
            _usbRootPath =  device.usbRootPath();
        }
    }

    emit initFinished();
    _isInPolling = false;
}

QString DeviceStorage::usbRootPath()
{
    return _usbRootPath;
}

DeviceStorage::DeviceModel DeviceStorage::DeviceModel::Parse(const QString &l)
{
    if(!l.startsWith("usbdrive")) return {};

    auto words=l.split('|');
    if(words.count()<1) return {};

    auto words0 = words[0].split(':');
    if(words0.count()<4) return {};

    DeviceModel d {
        .devPath=words0[1],
        .usbPath=words0[2],
        .serial=words0[3],
        .partitions = {}
    };

    for(int i=1;i<words.count();i++){
        PartitionModel p = PartitionModel::Parse(words[i]);
        if(!p.partPath.isEmpty()){
            d.partitions.append(p);
        }
    }

    return d;
}

QString DeviceStorage::DeviceModel::toString()
{
    QString txt = usbPath;
    QString ptxt;
    for(auto&a:partitions){
        if(!ptxt.isEmpty())ptxt+=":";
        ptxt+=a.toString();
    }
    if(!ptxt.isEmpty()) txt+=":"+ptxt;
    return txt;
}

//1-1.2.4_1.0
QString DeviceStorage::DeviceModel::usbRootPath()
{
    auto a = usbPath.split('_');
    int ix = a.first().lastIndexOf('.');
    if(ix==1)return {};

    return a[0].mid(0,ix);
}

DeviceStorage::PartitionModel DeviceStorage::PartitionModel::Parse(const QString &l)
{
    auto words=l.split(':');
    if(words.count()<2) return {};

    PartitionModel p{
        .partPath=words[0],
        .label=words[1]
    };
    if(words.count()>=3) p.project=words[2];
    if(words.count()>=4) p.bin=words[3];
    return p;
}

QString DeviceStorage::PartitionModel::toString() const
{
    QString txt;
    if(!bin.isEmpty()){
        txt=bin;
    } else{
        if(!project.isEmpty()){
            txt=project;
        } else{
            if(!label.isEmpty()){
                txt=label;
            } else{
                txt=partPath;
            }
        }
    }
    return txt;
}
