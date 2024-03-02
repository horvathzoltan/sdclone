#include "mainpresenter.h"
#include "mainviewmodel.h"
#include "dowork.h"

#include <QFileDialog>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>



#include <bi/imagestorage.h>

MainPresenter::MainPresenter(QObject *parent) :QObject(parent)
{

}

void MainPresenter::appendView(IMainView *w)
{
    if(_views.contains(w)) return;
    _views.append(w);

    auto *view_obj = dynamic_cast<QObject*>(w);

    QObject::connect(view_obj, SIGNAL(PushButtonActionTriggered(IMainView *)),
                     this, SLOT(processPushButtonAction(IMainView *)));

    refreshView(w);
}

void MainPresenter::refreshView(IMainView *w)
{

    QString partLabel = "butyok2";

    _imageStorage.Init(partLabel);

    switch(_imageStorage.err()){
    case ImageStorage::OK:
    {
        w->set_StatusLine({"mounted:"+partLabel+" to:"+_imageStorage.mountPoint()});
        w->set_StorageLabel({partLabel+": "+_imageStorage.mountPoint()});
        QString imageFolder = "clone";
        QStringList e = _imageStorage.GetImageFilePaths(imageFolder);
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
    auto devices = _deviceStorage.devices();
    if(devices.isEmpty()){
        w->set_StatusLine({"usb devices not found"});
    } else{
        QStringList deviceList;
        for(auto&a:devices){
            QString txt = a.toString();
            deviceList.append(txt);
        }
        w->set_DeviceList({deviceList});
    }
}

void MainPresenter::initView(IMainView *w) const {
    MainViewModel::DoWorkRModel rm{"1"};
    w->set_DoWorkRModel(rm);

};

void MainPresenter::processPushButtonAction(IMainView *sender){
    qDebug() << "processPushButtonAction";
    auto m = sender->get_DoWorkModel();
    auto rm = DoWork::Work1(m);
    sender->set_DoWorkRModel(rm);
}

