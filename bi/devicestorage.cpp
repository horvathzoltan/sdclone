#include "devicestorage.h"

#include <helpers/filehelper.h>
#include <helpers/processhelper.h>

#include <QDir>
#include <QElapsedTimer>
#include <QThread>

extern ProcessHelper _processHelper;


DeviceStorage::DeviceStorage(QObject *parent) :QObject(parent)
{
    _pollingProcessHelper.SetPassword("Aladar123");
    QObject::connect(&_pollingProcessHelper, &ProcessHelper::finished,
                     this, &DeviceStorage::finished);
}

void DeviceStorage::GetPartitionData(DeviceModel *device)
{
    QString cmd= QStringLiteral("/home/pi/readsd/bin/readsd -m %1 -s Aladar123").arg(device->devPath);
    auto out = _pollingProcessHelper.ShellExecute(cmd);

    bool valid = out.exitCode==0 && !out.stdOut.isEmpty();
    if(valid) {
        QStringList lines = out.stdOut.split('\n');
        for (QString &l : lines) {
            if (l.isEmpty()) continue;
            if(!l.startsWith("usbdrive")) return;

            auto words=l.split('|');
            if(words.count()<1) return;

            for(int i=1;i<words.count();i++){
                PartitionModel p = PartitionModel::Parse(words[i]);
                if(!p.partPath.isEmpty()){
                    device->partitions.append(p);
                }
            }
        }
    }
}


DeviceStorage::DeviceModel DeviceStorage::Add(const QString &sysPath)
{
    QFileInfo fi(sysPath);

    DeviceModel dev = CreateDevice_BySys(fi);
    if(dev.isValid()){
        _devices.append(dev);
        return dev;
    }

    return {};
}

DeviceStorage::DeviceModel DeviceStorage::Update(const QString &devPath)
{
    //QFileInfo fi(sysPath);

    int ix = indexOf(devPath);
    if(ix>=0){
        DeviceModel& device = _devices[ix];
        device.partitions.clear();
        device.size = 0;
        ix = device.devPath.lastIndexOf('/');
        if(ix>0){
            QThread::msleep(300);

            QString devname = device.devPath.mid(ix+1);
            device.size = GetSize(devname);

            if(device.size>0){
                GetPartitionData(&device);
            }
            return device;
        }
    }

    return {};
}

void DeviceStorage::Remove(const QString &devPath)
{
    int ix = indexOf(devPath);
    if(ix>=0){
        _devices.removeAt(ix);
    }
}

int DeviceStorage::indexOf(const QString &devPath){
    for(int i=0;i<_devices.length();i++){
        DeviceModel m = _devices[i];
        if(m.devPath==devPath) return i;
    }
    return -1;
}

// DeviceStorage::DeviceStorage() {
//     _pollingProcessHelper.SetPassword("Aladar123");
//     QObject::connect(&_pollingProcessHelper, &ProcessHelper::finished,
//                      this, &DeviceStorage::finished);
// }

/*
 *usb: 0:1.2.2.:1.0
 *
*/
void DeviceStorage::Init2(){
    auto fileInfoList = FileHelper::GetSystemFiles("/dev/disk/by-path", {});
    //QStringList devicePaths = FileHelper::GetRootList(fileInfoList, "usb-0:1.2");

    if(_usbRootPath.isEmpty()) return;
    _devices.clear();
    //_usbRootPath = "";
    QString filter = "usb-"+_usbRootPath;
    for(auto&fi:fileInfoList){
        QString fileName = fi.fileName();
        if(!fileName.contains(filter)) continue;
        DeviceModel device = CreateDevice(fi);
        if(device.isValid()){
            _devices.append(device);
        }
    }

    emit initFinished();
    _isInPolling = false;
}
// "add@/devices/platform/scb/fd500000.pcie/pci0000:00/0000:00:00.0/0000:01:00.0/usb1/1-1/1-1.2/1-1.2.1/1-1.2.1:1.0/host2/target2:0:0/2:0:0:0/block/sdc"
DeviceStorage::DeviceModel DeviceStorage::CreateDevice_BySys(const QFileInfo& fi){
    QString fileName = fi.canonicalFilePath();

    QString devFn = fi.fileName();
    int ix = fileName.indexOf("/usb");
    if(ix<=0) return {};
    int ix2 = fileName.indexOf("/host");
    if(ix2<=0) return {};
    QString brutty = fileName.mid(ix+1,ix2-ix-1);
    ix = brutty.lastIndexOf('/');
    QString usbPath = brutty.mid(ix+1);

    DeviceModel device;
    device.devPath = "/dev/"+devFn;
    device.usbPath = GetUsbPath(usbPath);

    device.size = GetSize(devFn);

    if(device.size>0){
        QThread::msleep(300);
        GetPartitionData(&device);
    }
    return device;
}

QString DeviceStorage::GetUsbPath(const QString& str){
    int ix = str.indexOf('-');
    if(ix<=0){
        ix = str.indexOf(':');
    }
    if(ix<=0) return {};
    QString u = str.mid(ix+1);
    return u;
}

DeviceStorage::DeviceModel DeviceStorage::CreateDevice(const QFileInfo& fi){
    //QFileInfo fi(fileName);
    QString fileName = fi.fileName();

    auto target = fi.symLinkTarget();
    if(target.isEmpty()) return {};
    int L = target.length();
    QChar l = target.at(L-1);
    if(l.isDigit()) return {};

    QStringList tokens = fileName.split('-');
    if(tokens.length()<6) return{};
    DeviceModel device;// = DeviceModel::Parse(l);
    device.devPath = target;
    device.usbPath = GetUsbPath(tokens[5]);//.replace(':','_');

    QFileInfo ff(target);

    auto s = ff.fileName();
    device.size = GetSize(s);

    if(device.size>0){
        GetPartitionData(&device);
    }
    return device;
}

QFileInfo DeviceStorage::FindByPath(const QString& devPath){
    if(devPath.isEmpty()) return{};

    auto fileInfoList = FileHelper::GetSystemFiles("/dev/disk/by-path", {});

    for(auto&fi:fileInfoList){
        QString target = fi.canonicalFilePath();
        if(target == devPath) return fi;
    }
    return {};
}

QString DeviceStorage::DeviceModel::serial() const{
    QString s;
    s+=usbPath;
    for(auto&p:partitions){
        s+="_"+p.label+"_"+p.project;
    }
    return s;
}


int DeviceStorage::GetSize(const QString& d){
    QFile f("/sys/block/"+d+"/size");
    f.open(QFile::OpenModeFlag::ReadOnly);
    QFile::FileError err = f.error();
    if(err!=QFile::FileError::NoError) return 0;
    QByteArray b = f.readLine();
    err = f.error();
    if(err!=QFile::FileError::NoError) return 0;
    f.close();

    auto es = f.errorString();
    if(!b.isEmpty()){
        bool ok;
        QString c(b);
        quint64 r = c.toULong(&ok);
        if(ok){
            return r;
        }
    }
    return 0;
}

void DeviceStorage::Init()
{
    if(_isInPolling) return;
    _isInPolling = true;
    _pollingCounter++;    

    QString cmd= QStringLiteral("/home/pi/readsd/bin/readsd -q -s Aladar123");
    _pollingProcessHelper.ShellExecuteNoWait(cmd);
}

void DeviceStorage::finished()
{
    _devices.clear();
    //_usbRootPath = "";

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
            //_usbRootPath =  device.usbRootPath();
        }
    }

    emit initFinished();
    _isInPolling = false;
}

// QString DeviceStorage::usbRootPath()
// {
//     return _usbRootPath;
// }

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
        //.serial=words0[3],
        .size = 0,
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
    auto words=l.split(',');
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
