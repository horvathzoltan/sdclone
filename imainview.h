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
    //virtual void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m) = 0;
    //virtual MainViewModel::DoWorkModel get_DoWorkModel() =0;
    virtual void set_StatusLine(const MainViewModel::StringModel& m) = 0;
    virtual void set_StorageLabel(const MainViewModel::StringModel& m) = 0;
    virtual void set_ImageFileList(const MainViewModel::StringListModel& m) = 0;
    virtual void set_DeviceList(const MainViewModel::DeviceListModel& m) = 0;

public: // signals
    virtual void ReadActionTriggered(IMainView *sender) = 0;
};

#endif // IMAINVIEW_H
