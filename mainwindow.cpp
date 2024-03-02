#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_DoWorkRModel(const MainViewModel::DoWorkRModel& m)
{
    ui->label->setText(m.txt);
}

void MainWindow::set_StatusLine(const MainViewModel::StringModel &m)
{
    static QString a;
    a+=m.txt+'\n';
    ui->label_stateline->setText(a);
}

void MainWindow::set_StorageLabel(const MainViewModel::StringModel &m)
{
    ui->label_storage->setText(m.txt);
}

void MainWindow::set_ImageFileList(const MainViewModel::StringListModel& m)
{
    ui->listWidget_images->clear();
    if(!m.txts.isEmpty()){
        ui->listWidget_images->addItems(m.txts);
    }
}

void MainWindow::set_DeviceList(const MainViewModel::StringListModel &m)
{
    ui->listWidget_devices->clear();
    if(!m.txts.isEmpty()){
        ui->listWidget_devices->addItems(m.txts);
    }
}

auto MainWindow::get_DoWorkModel() -> MainViewModel::DoWorkModel
{
    auto t = ui->label->text();
    bool isok;
    int i = t.toInt(&isok, 10);
    if(!isok) return {-1};
    return {i};
};

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "PushButtonActionTriggered";
    emit PushButtonActionTriggered(this);
}

