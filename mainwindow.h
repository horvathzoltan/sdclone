#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imainview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IMainView
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m);
    void set_StatusLine(const MainViewModel::StringModel& m);
    void set_StorageLabel(const MainViewModel::StringModel& m);
    void set_ImageFileList(const MainViewModel::StringListModel& m);
    void set_DeviceList(const MainViewModel::StringListModel& m);

    MainViewModel::DoWorkModel get_DoWorkModel();
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void PushButtonActionTriggered(IMainView *sender);
private slots:    
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
