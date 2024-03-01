#ifndef IMAINVIEW_H
#define IMAINVIEW_H

#include "mainviewmodel.h"

#include <QColor>
#include <QPointF>
#include <QRect>
#include <QSet>

class IMainView
{
public:
    virtual void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m) = 0;
    virtual MainViewModel::DoWorkModel get_DoWorkModel() =0;

public: // signals
    virtual void PushButtonActionTriggered(IMainView *sender) = 0;
};

#endif // IMAINVIEW_H
