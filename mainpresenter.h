#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

//#include "actions.h"
//#include "updates.h"

#include <QList>
#include <QObject>
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

    void refreshView(IMainView *w) const;


private slots:
    void processPushButtonAction(IMainView *sender);
};

#endif // MAINPRESENTER_H
