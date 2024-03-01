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
