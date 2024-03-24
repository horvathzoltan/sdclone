#include "mainpresenter.h"
#include "mainviewmodel.h"
//#include "dowork.h"

#include <QFileDialog>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QElapsedTimer>



#include <bi/imagestorage.h>

#include <helpers/processhelper.h>

extern ProcessHelper _processHelper;

MainPresenter::MainPresenter(QObject *parent) :QObject(parent)
{
    connect(&_devicePollTimer, &QTimer::timeout, this, [this](){
        if(!_isprocessWriteAction && !_isprocessReadAction){
            deviceStorageRefresh();
        }
    });

    QObject::connect(&_deviceStorage, SIGNAL(initFinished()),
                     this, SLOT(processInitFinished()));

}

void MainPresenter::appendView(IMainView *w)
{
    if(_views.contains(w)) return;
    _views.append(w);

    auto *view_obj = dynamic_cast<QObject*>(w);

    QObject::connect(view_obj, SIGNAL(ReadActionTriggered(IMainView*)),
                     this, SLOT(processReadAction(IMainView*)));

    QObject::connect(view_obj, SIGNAL(WriteActionTriggered(IMainView*)),
                     this, SLOT(processWriteAction(IMainView*)));

    refreshView(w);
}

void MainPresenter::refreshView(IMainView *w)
{
    Q_UNUSED(w)
}

void MainPresenter::initView(IMainView *w) {
//    MainViewModel::DoWorkRModel rm{"1"};
//    w->set_DoWorkRModel(rm);

    QString partLabel = "butyok2";

    _imageStorage.Init(partLabel);

    switch(_imageStorage.err()){
    case ImageStorage::OK:
    {
        w->set_StatusLine({"mounted:"+partLabel+" to:"+_imageStorage.mountPoint()});
        QString imageFolder = "clone";
        w->set_StorageLabel({imageFolder+"@"+_imageStorage.mountPoint()});

        _imageStorage.SetImageFilePath(imageFolder);
        QStringList e = _imageStorage.GetImageFilePaths();
        if(e.isEmpty()){
            w->set_StatusLine({"images not found:"+imageFolder});
        } else{
            w->set_ImageFileList({e});
        }
        break;
    }
    case ImageStorage::NoDevPath:
        w->set_StatusLine({"device not found:"+partLabel});
        w->set_StorageLabel({""});
        w->set_ImageFileList({});
        break;
    case ImageStorage::CannotMount:
        w->set_StatusLine({"cannot mount partition:"+partLabel+" to:"+_imageStorage.mountPoint()});
        w->set_StorageLabel({""});
        w->set_ImageFileList({});
        break;
    }

    //deviceStorageRefresh();
    _devicePollTimer.start(2000);
};

void MainPresenter::deviceStorageRefresh()
{
    _deviceStorage.Init();
}

void MainPresenter::processInitFinished()
{
    QList<DeviceStorage::DeviceModel> devices = _deviceStorage.devices();
    // itt kell a disk serial
    // ami nincs benne a listboxban, és most van, azt bele kell tenni
    // ami benne van a listboxban, és most nincs benne azt ki kell venni
    // mi selected, azt uuid alapján selectelni kell - illetve az elvileg úgy fog magától maradni
    //_views[0]->set_DeviceListClear();
    if(devices.isEmpty()){
        _views[0]->set_StatusLine({"usb devices not found"});
    } else{
        MainViewModel::DeviceListModel deviceListWm = MainPresenter::DeviceModelToWm(devices);
        _views[0]->set_DeviceList(deviceListWm);
    }
}


MainViewModel::DeviceListModel MainPresenter::DeviceModelToWm(const QList<DeviceStorage::DeviceModel> &devices)
{
    MainViewModel::DeviceListModel m;

    for(auto&device:devices){
        MainViewModel::DeviceModel d;

        d.deviceLabel = device.usbPath;
        d.usbDevicePath = device.usbPath;
        //d.serial = device.serial;
        QString o;
        QString s;
        s+=device.usbPath;
        for(auto&p:device.partitions){
            d.partitionLabels.append(p.toString());
            s+="_"+p.label+"_"+p.project;
            if(o.isEmpty()){
                if(!p.project.isEmpty()){
                    o = p.project;
                } else{
                    o = p.label;
                }
            }                        
        }
        d.outputFileName = o;
        d.serial = s;

        m.devices.append(d);
    }
    return m;
}



/*READ*/

void MainPresenter::processReadAction(IMainView *sender)
{
    if(!_isprocessReadAction && !_isprocessWriteAction){
        _isprocessReadAction = true;
        qDebug() << "processReadAction";
        sender->set_StatusLine({"processReadAction"});

        MainViewModel::DeviceModel a = sender->get_Device();

        sender->set_StatusLine({"usbDevicePath:"+a.usbDevicePath});
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmmss"));
        QString o = a.outputFileName+"_"+timestamp;
        sender->set_StatusLine({"outputFileName:"+o});

        QString cmd = QStringLiteral("/home/pi/readsd/bin/readsd -p %1 -o %2 -s Aladar123 -f -u %3")
                        .arg(_imageStorage.imageFolder(),o,a.usbDevicePath);

        qDebug()<<"cmd:"+cmd;
        _t.start();

        QObject::connect(&_processHelper, &ProcessHelper::stdErrR,
                       this, &MainPresenter::stdErrReader);
        QObject::connect(&_processHelper, &ProcessHelper::finished,
                          this, &MainPresenter::finished);

        _processHelper.ShellExecuteSudoNoWait(cmd);
    }
}


void MainPresenter::stdErrReader(QByteArray&d)
{
    _stdErr.append(d);
    // ha "B/s" van a végén, a dd küldött statsot

    QString g;
    _t.restart();

    int ix = _stdErr.lastIndexOf('\n');
    if(ix!=-1){
        g = _stdErr.mid(0,ix);
        _stdErr = _stdErr.mid(ix+1);
    } else{
        g = QString(_stdErr);
        _stdErr.clear();
    }

    if(!g.isEmpty()){
        _views[0]->set_StatusLine({g});
    }
}


void MainPresenter::finished()
{
    QObject::disconnect(&_processHelper, &ProcessHelper::stdErrR,
                     this, &MainPresenter::stdErrReader);
    QObject::disconnect(&_processHelper, &ProcessHelper::finished,
                     this, &MainPresenter::finished);

    if(_isprocessReadAction)
        _isprocessReadAction = false;
    if(_isprocessWriteAction)
        _isprocessWriteAction = false;

    QString g = QString(_stdErr);
    _stdErr.clear();
    if(!g.isEmpty()){
        _views[0]->set_StatusLine({g});
    }
    _views[0]->set_StatusLine({"finished"});
}

/*WRITE*/
//./writesd2 -p /media/pi/butyok2/clone/ -i img57 -s Aladar123 -f -u 1-1.2

void MainPresenter::processWriteAction(IMainView *sender)
{
    if(!_isprocessWriteAction && !_isprocessReadAction){
        _isprocessWriteAction = true;
        qDebug() << "processWriteAction";
        sender->set_StatusLine({"processWriteAction"});

        QString usbDevicePath = _deviceStorage.usbRootPath();

        auto m = sender->get_InputFileName();

        QString cmd = QStringLiteral("/home/pi/writesd2/bin/writesd2 -p %1 -i %2 -s Aladar123 -f -u %3")
                          .arg(_imageStorage.imageFolder(),m.txt,usbDevicePath);

        qDebug()<<"cmd:"+cmd;
        _t.start();

        QObject::connect(&_processHelper, &ProcessHelper::stdErrR,
                         this, &MainPresenter::stdErrReader);
        QObject::connect(&_processHelper, &ProcessHelper::finished,
                         this, &MainPresenter::finished);

        _processHelper.ShellExecuteSudoNoWait(cmd);
    }
}
