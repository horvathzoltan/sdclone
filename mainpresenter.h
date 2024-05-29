#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

//#include "actions.h"
//#include "updates.h"

#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QList>
#include <QObject>
#include <QTimer>
#include <bi/devicestorage.h>
#include <bi/imagestorage.h>
#include "imainview.h"

class IMainView;

class MainPresenter : public QObject
{
    Q_OBJECT

public:
    QElapsedTimer _t;
    explicit MainPresenter(QObject *parent = nullptr);
    void appendView(IMainView *w);
    void initView(IMainView *w);
    static int findFirstDiffPos(const QString &a, const QString &b);
private:
    QByteArray _stdErr;
    QList<IMainView*> _views;
    ImageStorage _imageStorage;
    DeviceStorage _deviceStorage;
    MainViewModel::WriteStatusWM _wm;
    qlonglong _writeBytesAll=0;
    int _writeBlockSize=0;
    qlonglong _writeBytes=0;
    qlonglong _writeBytes2=0;
    QElapsedTimer _writeTimer;

    void SetRemainingTime();
    void SetTotalTime();

    void refreshView(IMainView *w);

    static MainViewModel::DeviceListModel DeviceModelToWm(
        const QList<DeviceStorage::DeviceModel>& devices,
        const QString& usbRootPath);

    static QString GetSerial(const DeviceStorage::DeviceModel &device);


    //QTimer _devicePollTimer;
    //QTimer _imageFolderPollTimer;
    //bool isFreeForOperation();
    quint32 _pollingCounter = 0;
    quint32 _imagePollingCounter = 0;

    void Write();
    void Read();
    void PollDevices();
    void PollImages();
    void Exit();
    void ProcessWriteResult();

    class PresenterState{
    public:
        enum State:int{None=0,Write,Read,waitForWrite,waitForRead,Exit};//,PollImages,PollDevices

        void handleInput(MainPresenter* presenter, State s);
        State state(){return _state;}
    private:
        State _state=None;

    };

    PresenterState _presenterState;

    void RefreshImageFolder();
    QFileSystemWatcher watcher_images;
    QFileSystemWatcher watcher_devices;

private slots:
    void processReadAction(IMainView *sender);
    void processWriteAction(IMainView *sender);
    void processInitFinished();
    void processExitAction(IMainView *sender);
public slots:
    void stdErrReader(QByteArray&d);
    void finished();    
};

#endif // MAINPRESENTER_H
