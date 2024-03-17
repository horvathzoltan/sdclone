#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

//#include "actions.h"
//#include "updates.h"

#include <QElapsedTimer>
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
private:
    QByteArray _stdErr;

    QList<IMainView*> _views;

    ImageStorage _imageStorage;
    DeviceStorage _deviceStorage;

    void refreshView(IMainView *w);

    static MainViewModel::DeviceListModel DeviceModelToWm(const QList<DeviceStorage::DeviceModel>& devices);

    bool _isprocessReadAction = false;
    bool _isprocessWriteAction = false;

    void deviceStorageRefresh();

    QTimer _devicePollTimer;

private slots:
    void processReadAction(IMainView *sender);
    void processWriteAction(IMainView *sender);
public slots:
    void stdErrReader(QByteArray&d);
    void finished();
};

#endif // MAINPRESENTER_H
