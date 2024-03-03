#include "devicestorage.h"

#include <helpers/processhelper.h>

DeviceStorage::DeviceStorage() {}

void DeviceStorage::Init()
{
    _devices.clear();
    QString cmd= QStringLiteral("/home/pi/readsd/bin/readsd -q -s Aladar123");
    auto out = ProcessHelper::ShellExecute(cmd);

    if(out.exitCode!=0) return;
    if(out.stdOut.isEmpty()) return;

    QStringList lines = out.stdOut.split('\n');
    for(auto&l:lines)
    {
        if(l.isEmpty()) continue;

        DeviceModel d = DeviceModel::Parse(l);
        if(!d.devPath.isEmpty()){
            _devices.append(d);
        }
    }
}

DeviceStorage::DeviceModel DeviceStorage::DeviceModel::Parse(const QString &l)
{
    if(!l.startsWith("usbdrive")) return {};

    auto words=l.split('|');
    if(words.count()<1) return {};

    auto words0 = words[0].split(':');
    if(words0.count()<3) return {};

    DeviceModel d {
        .devPath=words0[1],
        .usbPath=words0[2],
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
