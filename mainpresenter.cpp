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

bool MainPresenter::_isprocessReadAction = false;

MainPresenter::MainPresenter(QObject *parent) :QObject(parent)
{

}

void MainPresenter::appendView(IMainView *w)
{
    if(_views.contains(w)) return;
    _views.append(w);

    auto *view_obj = dynamic_cast<QObject*>(w);

    QObject::connect(view_obj, SIGNAL(ReadActionTriggered(IMainView*)),
                     this, SLOT(processReadAction(IMainView*)));

    // QObject::connect(&_processHelper, &ProcessHelper::stdErrR,
    //                  this, &MainPresenter::stdErrReader);

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

    _deviceStorage.Init();
    QList<DeviceStorage::DeviceModel> devices = _deviceStorage.devices();
    if(devices.isEmpty()){
        w->set_StatusLine({"usb devices not found"});
    } else{
        MainViewModel::DeviceListModel deviceListWm = MainPresenter::DeviceModelToWm(devices);
        w->set_DeviceList(deviceListWm);
    }
};

MainViewModel::DeviceListModel MainPresenter::DeviceModelToWm(const QList<DeviceStorage::DeviceModel> &devices)
{
    MainViewModel::DeviceListModel m;

    for(auto&device:devices){
        MainViewModel::DeviceModel d;

        d.deviceLabel = device.usbPath;
        d.usbDevicePath = device.usbPath;
        QString o;
        for(auto&p:device.partitions){
            d.partitionLabels.append(p.toString());
            if(o.isEmpty()){
                if(!p.project.isEmpty()){
                    o = p.project;
                } else{
                    o = p.label;
                }
            }
        }
        d.outputFileName = o;

        m.devices.append(d);
    }
    return m;
}

void MainPresenter::processReadAction(IMainView *sender)
{
    if(!_isprocessReadAction){
        _isprocessReadAction = true;
        qDebug() << "processReadAction";
        sender->set_StatusLine({"processReadAction"});

        MainViewModel::DeviceModel a = sender->get_Device();

        sender->set_StatusLine({"usbDevicePath:"+a.usbDevicePath});
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmmss"));
        QString o = a.outputFileName+"_"+timestamp;
        sender->set_StatusLine({"outputFileName:"+o});

        QString cmd = QStringLiteral("/home/pi/readsd/bin/readsd -p %1 -o %3 -s Aladar123 -f -u %2")
                        .arg(_imageStorage.imageFolder()).arg(a.usbDevicePath).arg(o);

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
    static QByteArray b;
    b.append(d);
    // ha "B/s" van a végén, a dd küldött statsot

    //if(_t.elapsed()>1000){
    // QList<QByteArray> a = b.split('\n');
    // QString g;
    // for(int i=a.count()-1;i>=0;i--){
    //     g = a[i].trimmed();
    //     if(!g.isEmpty()){
    //         break;
    //     }
    // }
    QString g(b);
    _t.restart();
    b.clear();
    if(!g.trimmed().isEmpty()){
        _views[0]->set_StatusLine({g});
    }

    //}
}


void MainPresenter::finished()
{
    QObject::disconnect(&_processHelper, &ProcessHelper::stdErrR,
                     this, &MainPresenter::stdErrReader);
    QObject::disconnect(&_processHelper, &ProcessHelper::finished,
                     this, &MainPresenter::finished);

    _isprocessReadAction = false;
    _views[0]->set_StatusLine({"finished"});
}

