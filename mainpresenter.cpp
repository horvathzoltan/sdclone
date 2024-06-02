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
#include <QCoreApplication>



#include <bi/imagestorage.h>

#include <helpers/filehelper.h>
#include <helpers/processhelper.h>

extern ProcessHelper _processHelper;

MainPresenter::MainPresenter(QObject *parent) :QObject(parent)
{
    // connect(&_devicePollTimer, &QTimer::timeout, this, [this](){
    //     _pollingCounter++;
    //     _presenterState.handleInput(this, PresenterState::PollDevices);
    // });

/*    connect(&_imageFolderPollTimer, &QTimer::timeout, this, [this](){
         _imagePollingCounter++;
         _presenterState.handleInput(this, PresenterState::PollImages);
    });*/

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

    QObject::connect(view_obj, SIGNAL(ExitActionTriggered(IMainView*)),
                     this, SLOT(processExitAction(IMainView*)));

    refreshView(w);
}

void MainPresenter::refreshView(IMainView *w)
{
    Q_UNUSED(w)
}

void MainPresenter::initView(IMainView *w) {
//    MainViewModel::DoWorkRModel rm{"1"};
//    w->set_DoWorkRModel(rm);

    //RefreshImageFolder();
    //_presenterState.handleInput(this, PresenterState::PollImages);
    QString partLabel = "butyok2";
    _imageStorage.Init(partLabel);

    switch(_imageStorage.err()){
    case ImageStorage::OK:
    {
        auto mountpoint = _imageStorage.mountPoint();
        _views[0]->set_StatusLine({"mounted:"+partLabel+" to:"+mountpoint});
        QString imageFolder = "clone";
        _imageStorage.SetImageFilePath(imageFolder);
        _views[0]->set_StorageLabel({imageFolder+"@"+mountpoint});

        PollImages();

        auto i2mageFolder = _imageStorage.imageFolder();
        watcher_images.addPath(i2mageFolder);
        QObject::connect(&watcher_images, &QFileSystemWatcher::directoryChanged, [this](){PollImages();});
        break;
    }
    case ImageStorage::NoDevPath:
        _views[0]->set_StatusLine({"device not found:"+partLabel});
        _views[0]->set_StorageLabel({""});
        _views[0]->set_ImageFileList({});
        break;
    case ImageStorage::CannotMount:
        _views[0]->set_StatusLine({"cannot mount partition:"+partLabel+" to:"+_imageStorage.mountPoint()});
        _views[0]->set_StorageLabel({""});
        _views[0]->set_ImageFileList({});
        break;
    }
    //_presenterState.handleInput(this, PresenterState::PollDevices);
    //_devicePollTimer.start(5000);
    //_imageFolderPollTimer.start(9000);

    //PollDevices();

    //watcher_devices.addPath("/dev/");
    //QObject::connect(&watcher_devices, &QFileSystemWatcher::directoryChanged, [this](){PollDevices();});
    //qw = new QDeviceWatcher;

    qw.appendEventReceiver(this);
    // QObject::connect(&qw,
    //                  &QDeviceWatcher::deviceAdded,
    //                  [this](){
    //                      _views[0]->set_StatusLine({"New device:" });
    //                  });
    QObject::connect(&qw,
                     SIGNAL(deviceAdded(QString)),
                     this,
                     SLOT(slotDeviceAdded(QString)),
                     Qt::DirectConnection);
    QObject::connect(&qw,
            SIGNAL(deviceChanged(QString)),
            this,
            SLOT(slotDeviceChanged(QString)),
            Qt::DirectConnection);
    QObject::connect(&qw,
            SIGNAL(deviceRemoved(QString)),
            this,
            SLOT(slotDeviceRemoved(QString)),
            Qt::DirectConnection);


    qw.start();
};

void MainPresenter::slotDeviceAdded(const QString& dev)
{
    //qDebug("tid=%#x: add %s", (quintptr) QThread::currentThreadId(), qPrintable(dev));

    //state->setText("<font color=#0000ff>Add: </font>" + dev);
    _views[0]->set_StatusLine({"New device:" +dev});
}

void MainPresenter::slotDeviceChanged(const QString& dev)
{
    //qDebug("tid=%#x: change %s", (quintptr) QThread::currentThreadId(), qPrintable(dev));

    //state->setText("<font color=#0000ff>Change: </font>" + dev);
    _views[0]->set_StatusLine({"Change device:"+ dev});
}

void MainPresenter::slotDeviceRemoved(const QString& dev)
{
    //qDebug("tid=%#x: remove %s", (quintptr) QThread::currentThreadId(), qPrintable(dev));

    //state->setText("<font color=#0000ff>Remove: </font>" + dev);
    _views[0]->set_StatusLine({"Remove device:"+ dev});
}

void MainPresenter::processWriteAction(IMainView *sender)
{
    _presenterState.handleInput(this, PresenterState::Write);
}

void MainPresenter::processReadAction(IMainView *sender)
{
    _presenterState.handleInput(this, PresenterState::Read);
}

void MainPresenter::processExitAction(IMainView *sender)
{
    _presenterState.handleInput(this, PresenterState::Exit);
}



void MainPresenter::RefreshImageFolder(){

    //QString partLabel = _imageStorage.partLabel();
    QString imageFolder = _imageStorage.imageFolder();
    switch(_imageStorage.err()){
    case ImageStorage::OK:
    {        
        QStringList e = _imageStorage.GetImageFilePaths();
        if(e.isEmpty()){
            _views[0]->set_StatusLine({"images not found:"+imageFolder});
            _views[0]->set_ClearImageFileList();
        } else{
            _views[0]->set_ImageFileList({e});
        }
        break;
    }
    default:break;
    // case ImageStorage::NoDevPath:
    //     _views[0]->set_StatusLine({"device not found:"+partLabel});
    //     _views[0]->set_StorageLabel({""});
    //     _views[0]->set_ImageFileList({});
    //     break;
    // case ImageStorage::CannotMount:
    //     _views[0]->set_StatusLine({"cannot mount partition:"+partLabel+" to:"+_imageStorage.mountPoint()});
    //     _views[0]->set_StorageLabel({""});
    //     _views[0]->set_ImageFileList({});
    //     break;
    }
}

//a = _deviceStorage.usbRootPath();

int MainPresenter::findFirstDiffPos(const QString& a, const QString& b) {
    if (a == b) return -1;
    int min = qMin(a.count(),b.count());
    int i;
    for(i=0;i<min;i++){
        if(a[i]!=b[i]) break;
    }
    return i;
}

MainViewModel::DeviceListModel MainPresenter::DeviceModelToWm(
    const QList<DeviceStorage::DeviceModel> &devices,
    const QString& usbRootPath)
{
    MainViewModel::DeviceListModel m;

    for (const DeviceStorage::DeviceModel &device : devices) {
        MainViewModel::DeviceModel d;

        int ix = findFirstDiffPos(device.usbPath, usbRootPath);
        QString label = (ix < 0) ? device.usbPath : device.usbPath.mid(ix);
        if (label.startsWith('.'))
            label = label.mid(1);
        ix = label.indexOf('_');
        if (ix > 0)
            label = label.left(ix);

        d.deviceLabel = label;
        d.usbDevicePath = device.usbPath;
        d.devicePath = device.devPath;
        // d.serial = device.serial;
        QString o;
        //QString s;
        //s += device.usbPath;
        for (auto &p : device.partitions) {
            d.partitionLabels.append(p.toString());
            //s += "_" + p.label + "_" + p.project;
            if (o.isEmpty()) {
                if (!p.project.isEmpty()) {
                    o = p.project;
                } else {
                    o = p.label;
                }
            }
        }
        d.outputFileName = o;
        d.serial = GetSerial(device);

        m.devices.append(d);
    }
    return m;
}

QString MainPresenter::GetSerial(const DeviceStorage::DeviceModel &device){
    QString s;
    s+=device.usbPath;
    for(auto&p:device.partitions){
        s+="_"+p.label+"_"+p.project;
    }
    return s;
}

// writing: xxx bytes (yy.0MB)
/*READ*/
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

    auto s = _presenterState.state();

    if(s==PresenterState::Write){
        if(!g.isEmpty()){

            QStringList lines = g.split('\n');
            for(auto&line:lines){
                if(line.startsWith("writing:")){
                    QStringList tokens = line.split(' ');
                    if(tokens.length()>=4){
                        bool ok;
                        qlonglong i = tokens[1].toLongLong(&ok);
                        if(ok){
                            _writeBytesAll = i;
                        }
                    }
                } else if(line.startsWith("cmd:dcfldd")){
                    QStringList tokens = line.split(' ');
                    for(auto&token:tokens){
                        if(token.startsWith("bs=")){
                            bool ok;
                            int i2 = token.mid(3).toInt(&ok);
                            if(ok){
                                _writeBlockSize=i2;
                            }
                        }
                    }
                } else if(line.endsWith("written.")){
                    QStringList tokens = line.split(' ');
                    if(tokens[1]=="blocks"){
                        bool ok;
                        qlonglong blocks = tokens[0].toLongLong(&ok);
                        if(ok){
                            _writeBytes = blocks*_writeBlockSize;

                            int percent = (50*_writeBytes)/_writeBytesAll;

                            _views[0]->set_ProgressLine({percent});
                            SetRemainingTime();
                        }
                    }
                } else if(line.endsWith("MB/s")){
                    QStringList tokens = line.split(' ');
                    //if(tokens[1]=="blocks"){
                        bool ok;
                        qlonglong bytes = tokens[0].toLongLong(&ok);
                        if(ok){
                            _writeBytes2 = bytes;

                            int percent = (50*(_writeBytes+_writeBytes2))/_writeBytesAll;

                            _views[0]->set_ProgressLine({percent});
                            SetRemainingTime();
                        }
                    //}
                }
            }
        }
    } else if(s==PresenterState::Read){
        if(!g.isEmpty()){

            QStringList lines = g.split('\n');
            for(auto&line:lines){
                if(line.startsWith("reading:")){
                    QStringList tokens = line.split(' ');
                    if(tokens.length()>=4){
                        bool ok;
                        qlonglong i = tokens[1].toLongLong(&ok);
                        if(ok){
                            _writeBytesAll = i;
                            _writeBlockSize = 512;
                        }
                    }
                } else if(line.startsWith("cmd:dcfldd")){
                    // QStringList tokens = line.split(' ');
                    // for(auto&token:tokens){
                    //     if(token.startsWith("bs=")){
                    //         bool ok;
                    //         int i2 = token.mid(3).toInt(&ok);
                    //         if(ok){
                    //             _writeBlockSize=i2;
                    //         }
                    //     }
                    // }
                } else if(line.endsWith("written.")){
                    // QStringList tokens = line.split(' ');
                    // if(tokens[1]=="blocks"){
                    //     bool ok;
                    //     qlonglong blocks = tokens[0].toLongLong(&ok);
                    //     if(ok){
                    //         _writeBytes = blocks*_writeBlockSize;

                    //         int percent = (50*_writeBytes)/_writeBytesAll;

                    //         _views[0]->set_ProgressLine({percent});
                    //     }
                    // }
                } else if(line.endsWith("MB/s")){
                    QStringList tokens = line.split(' ');
                    //if(tokens[1]=="blocks"){
                    bool ok;
                    qlonglong bytes = tokens[0].toLongLong(&ok);
                    if(ok){
                        if(bytes<_writeBytes)
                            _writeBytes2 = bytes;
                        else
                            _writeBytes = bytes;

                        int percent = (50*(_writeBytes+_writeBytes2))/_writeBytesAll;

                        _views[0]->set_ProgressLine({percent});

                        SetRemainingTime();
                    }
                    //}
                }
            }
        }
    }
}

void MainPresenter::SetRemainingTime(){
    qint64 t = _writeTimer.elapsed()/1000;
    if(t>0){
        qreal bps = (_writeBytes+_writeBytes2)/(t);
        qint64 remaining = (_writeBytesAll*2)-(_writeBytes+_writeBytes2);
        int rt = remaining/bps;

        QTime ts(0, 0, 0);
        ts = ts.addSecs(rt);
        QString t = ts.toString("hh:mm:ss");;
        _views[0]->set_ProgressText({t});
    }
}

void MainPresenter::SetTotalTime(){
    qint64 t = _writeTimer.elapsed();
    if(t>0){
        QTime ts(0, 0, 0);
        ts = ts.addMSecs(t);
        QString t = ts.toString("hh:mm:ss");;
        _views[0]->set_StatusLine({"total:"+t});
    }
}


void MainPresenter::finished()
{
    QObject::disconnect(&_processHelper, &ProcessHelper::stdErrR,
                     this, &MainPresenter::stdErrReader);
    QObject::disconnect(&_processHelper, &ProcessHelper::finished,
                     this, &MainPresenter::finished);

    QString g = QString(_stdErr);
    _stdErr.clear();
    if(!g.isEmpty()){
        _views[0]->set_StatusLine({g});
    }
    _views[0]->set_StatusLine({"finished"});

    _presenterState.handleInput(this, PresenterState::None);
}

/*WRITE*/
//./writesd2 -p /media/pi/butyok2/clone/ -i img57 -s Aladar123 -f -u 1-1.2


void MainPresenter::Read(){
    qDebug() << "processReadAction";
    _views[0]->set_StatusLine({"processReadAction"});
    _views[0]->set_ClearDeviceWriteStates();

    _writeBytesAll = 0;
    _writeBlockSize = 0;
    _writeBytes = 0;
    _writeBytes2 = 0;
    _writeTimer.restart();
    _views[0]->set_ShowProgressbar();

    MainViewModel::DeviceModel a = _views[0]->get_Device();

    if(a.usbDevicePath.isEmpty()){
        _views[0]->set_StatusLine({"no usbDevicePath"});
        _presenterState.handleInput(this, PresenterState::None);
        return;
    }

    _views[0]->set_StatusLine({"usbDevicePath:"+a.usbDevicePath});
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmmss"));
    QString o = a.outputFileName+"_"+timestamp;
    _views[0]->set_StatusLine({"outputFileName:"+o});

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

void MainPresenter::Write()
{
    qDebug() << "processWriteAction";
    _views[0]->set_StatusLine({"processWriteAction"});
    _views[0]->set_ClearDeviceWriteStates();
    _wm.clear();

    _writeBytesAll = 0;
    _writeBlockSize = 0;
    _writeBytes = 0;
    _writeBytes2 = 0;
    _writeTimer.restart();
    _views[0]->set_ShowProgressbar();

    QString usbDevicePath = _deviceStorage.usbRootPath();

    if(usbDevicePath.isEmpty()){
        _views[0]->set_StatusLine({"no usbDevicePath"});
        _presenterState.handleInput(this, PresenterState::None);
        return;
    }

    MainViewModel::StringModel m = _views[0]->get_InputFileName();

    if(m.txt.isEmpty()){
        _views[0]->set_StatusLine({"no inputFileName"});
        _presenterState.handleInput(this, PresenterState::None);
        return;
    }        


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

/*
pi@raspberrypi:~ $ ls /dev/sd*
/dev/sda  /dev/sda1  /dev/sdb  /dev/sdb1  /dev/sdc  /dev/sdd
pi@raspberrypi:~ $ ls /dev/sd*
/dev/sda  /dev/sda1  /dev/sdb  /dev/sdc  /dev/sdd
pi@raspberrypi:~ $ ls /dev/sd*
/dev/sda  /dev/sda1  /dev/sdc  /dev/sdd
pi@raspberrypi:~ $ ls /dev/sd*
/dev/sda  /dev/sda1  /dev/sdb  /dev/sdb1  /dev/sdc  /dev/sdd
pi@raspberrypi:~ $
 */

// /sys/block/sdc - size 0:nincs kártya
// udevadm info --query=property --path=/sys/block/sdc
void MainPresenter::PollDevices()
{
   /* auto fileInfoList = FileHelper::GetSystemFiles("/dev/disk/by-path", {});
    QStringList a = FileHelper::GetRootList(fileInfoList, "usb-0:1.2");

    auto b = _views[0]->get_DeviceList();*/
    _deviceStorage.Init();
}


void MainPresenter::PollImages()
{
    RefreshImageFolder();
    _presenterState.handleInput(this,PresenterState::None);
    //_deviceStorage.Init();
}

void MainPresenter::processInitFinished()
{
    QList<DeviceStorage::DeviceModel> devices = _deviceStorage.devices();   
    if(devices.isEmpty()){
        _views[0]->set_StatusLine({"usb devices not found"});
        _views[0]->set_DeviceListClear();
    } else{
        MainViewModel::DeviceListModel deviceListWm = MainPresenter::DeviceModelToWm(devices, _deviceStorage.usbRootPath());
        _views[0]->set_DeviceList(deviceListWm);
    }

    if(!_wm.isEmpty()){
        _views[0]->set_DeviceWriteStates(_wm);//beállítja a zöldet
        _wm.clear();
    }
    _presenterState.handleInput(this,PresenterState::None);

}



void MainPresenter::Exit()
{
    QCoreApplication::quit();
}

void MainPresenter::ProcessWriteResult(){    
    _wm = _views[0]->getLastWriteStatus();
    _views[0]->set_DeviceListClear();
    //_presenterState.handleInput(this, PresenterState::PollDevices);
    PollDevices();
}

void MainPresenter::PresenterState::handleInput(MainPresenter* presenter, State input)
{
    switch(_state){
    case None:
        switch(input){
        case Write:
            _state = input;
            presenter->_views[0]->set_PresenterStatus({"Write"});
            presenter->Write();
            break;
        case Read:
            _state = input;
            presenter->_views[0]->set_PresenterStatus({"Read"});
            presenter->Read();
            break;
        // case PollDevices:
        //     _state = input;
        //     presenter->_views[0]->set_PresenterStatus({"PollDevices"});
        //     presenter->PollDevices();
        //     break;
        // case PollImages:
        //     _state = input;
        //     presenter->_views[0]->set_PresenterStatus({"PollImages"});
        //     presenter->PollImages();
        //     break;
        case Exit:
            _state = input;
            presenter->_views[0]->set_PresenterStatus({"Exit"});
            presenter->Exit();
            break;
        default:break;
        }
        break;
    //case PollImages:
    // case PollDevices:
    //     switch(input){
    //     case Write:
    //         _state = waitForWrite;
    //         presenter->_views[0]->set_PresenterStatus({"waitForWrite"});
    //         break;
    //     case Read:
    //         _state = waitForRead;
    //         presenter->_views[0]->set_PresenterStatus({"waitForRead"});
    //         break;
    //     case None:
    //         _state = None;
    //         presenter->_views[0]->set_PresenterStatus({"None"});
    //         break;
    //     default:break;
    //     }
    //     break;
    case Write:
        switch(input){
        case None:
            _state = None;                        
            presenter->_views[0]->set_PresenterStatus({"None"});
            presenter->_views[0]->set_HideProgressbar();
            presenter->SetTotalTime();
            presenter->ProcessWriteResult();
            break;
        default:break;
        }
        break;
    case Read:
        switch(input){
        case None:
            _state = None;
            presenter->_views[0]->set_PresenterStatus({"None"});
            presenter->_views[0]->set_HideProgressbar();
            presenter->SetTotalTime();
            break;
        default:break;
        }
        break;
    case waitForWrite:
        switch(input){
        case None:
            _state = Write;
            presenter->_views[0]->set_PresenterStatus({"Write"});
            presenter->Write();
            break;
            default:break;
        }
        break;
    case waitForRead:
        switch(input){
        case None:
            _state = Read;
            presenter->_views[0]->set_PresenterStatus({"Read"});
            presenter->Read();
            break;
        default:break;
        }
        break;
    default:break;
    }

}
