#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "devicewidget.h"
#include "imainview.h"
#include "qlistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IMainView
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    //void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m);
    //MainViewModel::DoWorkModel get_DoWorkModel();
    void set_StatusLine(const MainViewModel::StringModel& m);
    void set_StorageLabel(const MainViewModel::StringModel& m);
    void set_ImageFileList(const MainViewModel::StringListModel& m);
    void set_DeviceList(const MainViewModel::DeviceListModel& m);
    void set_DeviceListClear();

    MainViewModel::DeviceModel get_Device();
    MainViewModel::StringModel get_InputFileName();

    bool devicesContainsBySerial(const QString& s);
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    DeviceWidget* CreateDeviceListItemWidget(const MainViewModel::DeviceModel &txt, const QSize &s);
signals:
    void ReadActionTriggered(IMainView *sender);
    void WriteActionTriggered(IMainView *sender);
private slots:    
    void on_pushButton_read_clicked();
    void on_pushButton_write_clicked();    
};
#endif // MAINWINDOW_H
