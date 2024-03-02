#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

//#include "actions.h"
//#include "updates.h"

#include <QList>
#include <QObject>
#include <bi/devicestorage.h>
#include <bi/imagestorage.h>
#include "imainview.h"

class IMainView;

class MainPresenter : public QObject
{
    Q_OBJECT

public:
    explicit MainPresenter(QObject *parent = nullptr);
    void appendView(IMainView *w);
    void initView(IMainView *w) const;
private:
    QList<IMainView*> _views;

    ImageStorage _imageStorage;
    DeviceStorage _deviceStorage;

    void refreshView(IMainView *w);


private slots:
    void processPushButtonAction(IMainView *sender);
};

#endif // MAINPRESENTER_H
