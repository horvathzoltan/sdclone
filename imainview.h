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
    virtual void set_PresenterStatus(const MainViewModel::StringModel& m) = 0;
    virtual void set_StorageLabel(const MainViewModel::StringModel& m) = 0;
    virtual void set_ImageFileList(const MainViewModel::StringListModel& m) = 0;
    virtual void set_DeviceList(const MainViewModel::DeviceListModel& m) = 0;
    virtual void set_DeviceListClear() = 0;
    virtual MainViewModel::WriteStatusWM getLastWriteStatus() = 0;
    virtual void set_DeviceWriteStates(const MainViewModel::WriteStatusWM& m) = 0;
    virtual void set_ClearDeviceWriteStates() = 0;
    virtual void set_ProgressLine(const MainViewModel::IntModel& m) = 0;
    virtual void set_ShowProgressbar() = 0;
    virtual void set_HideProgressbar() = 0;
    virtual void set_ProgressText(const MainViewModel::StringModel& m) = 0;

    virtual MainViewModel::StringModel get_InputFileName() = 0;
    virtual MainViewModel::IntModel get_WriteBytes() = 0;

    virtual MainViewModel::DeviceModel get_Device() = 0;

public: // signals
    virtual void ReadActionTriggered(IMainView *sender) = 0;
    virtual void WriteActionTriggered(IMainView *sender) = 0;
    virtual void ExitActionTriggered(IMainView *sender) = 0;
};

#endif // IMAINVIEW_H
