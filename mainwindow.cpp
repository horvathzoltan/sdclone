#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "devicewidget.h"

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

// void MainWindow::set_DoWorkRModel(const MainViewModel::DoWorkRModel& m)
// {
//     ui->label->setText(m.txt);
// }

// auto MainWindow::get_DoWorkModel() -> MainViewModel::DoWorkModel
// {
//     auto t = ui->label->text();
//     bool isok;
//     int i = t.toInt(&isok, 10);
//     if(!isok) return {-1};
//     return {i};
// };

void MainWindow::set_StatusLine(const MainViewModel::StringModel &m)
{
    static QStringList a;

    QStringList lines = m.txt.split('\n');
    for(auto&line:lines){

        if(line.startsWith('\r')){
            a.removeLast();
        }
        auto l = line.remove('\r').remove('\n');
        if(!l.isEmpty()){
            a.append(l);
        }
    }
    ui->plainTextEdit_status->setPlainText(a.join('\n'));
    ui->plainTextEdit_status->moveCursor(QTextCursor::End);
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

void MainWindow::set_DeviceList(const MainViewModel::DeviceListModel &m)
{
    static bool lock = false;
    if(lock) return;
    lock = true;
    set_StatusLine({"set_DeviceList"});
    //QList<QListWidgetItem*> itemsToDelete;
    QList<int> rowToDelete;

    for(int row = 0; row < ui->listWidget_devices->count(); row++){
        QListWidgetItem *item = ui->listWidget_devices->item(row);
        DeviceWidget *w = (DeviceWidget*)(ui->listWidget_devices->itemWidget(item));        
        if(w){
            bool contains = m.containsBySerial(w->_serial);

            if(!contains){
                //itemsToDelete.append(item);
                rowToDelete.append(row);
            }
        }
    }

    for (int row : rowToDelete) {

        QListWidgetItem *item = ui->listWidget_devices->takeItem(row);
        DeviceWidget *w = (DeviceWidget *)(ui->listWidget_devices->itemWidget(item));
        ui->listWidget_devices->removeItemWidget(item);
        delete w;
        delete item;
    }

    for(auto&device:m.devices){
        bool contains = devicesContainsBySerial(device.serial);
        if(!contains)
        {
            QSize s = ui->listWidget_devices->size();
            DeviceWidget *w = CreateDeviceListItemWidget(device, s);
            QListWidgetItem *item = new QListWidgetItem();

            item->setSizeHint(QSize(w->width(),w->height()));
            ui->listWidget_devices->addItem(item);
            ui->listWidget_devices->setItemWidget(item, w);

            set_StatusLine({"add serial:"+device.usbDevicePath});
        }
    }
    int a = ui->listWidget_devices->count();
    set_StatusLine({"items:"+QString::number(a)});
    lock = false;
}

bool MainWindow::devicesContainsBySerial(const QString &s)
{
    for(int row = 0; row < ui->listWidget_devices->count(); row++){
        QListWidgetItem *item = ui->listWidget_devices->item(row);
        DeviceWidget *w = (DeviceWidget*)(ui->listWidget_devices->itemWidget(item));

        if(w && w->_serial==s) return true;
    }
    return false;
}

void MainWindow::set_DeviceListClear()
{
    ui->listWidget_devices->clear();
}

MainViewModel::DeviceModel MainWindow::get_Device()
{
    MainViewModel::DeviceModel d;
    QList<QListWidgetItem *> selectedItems = ui->listWidget_devices->selectedItems();
    if(!selectedItems.isEmpty()){
        QListWidgetItem *item = selectedItems[0];
        QWidget *a = ui->listWidget_devices->itemWidget(item);
        DeviceWidget *w = static_cast<DeviceWidget*>(a);
        if(w){
            d.usbDevicePath = w->_usbDevicePath;
            d.outputFileName = w->_outputFileName;
        }
    }
    return d;
}

DeviceWidget* MainWindow::CreateDeviceListItemWidget(const MainViewModel::DeviceModel& device, const QSize& s)
{
    int width = s.width()-8;
    int height = 80;
    int l1_width = s.width()/2;
    int l2_width = s.width()-l1_width;

    QHBoxLayout *lay= new QHBoxLayout();
    lay->setGeometry(QRect(0, 0, width, height));
    lay->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    QVBoxLayout *lay2= new QVBoxLayout();
    lay->setGeometry(QRect(0, 0, width, height));
    lay->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    QLabel *l1 = new QLabel();
    l1->setGeometry(QRect(0, 0, l1_width, height/2));
    l1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l1->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    QFont f1 = l1->font();
    f1.setPointSize(10);
    l1->setFont(f1);

    l1->setText(device.deviceLabel);

    QLabel *l0 = new QLabel();
    l0->setGeometry(QRect(0, 0, l1_width, height/2));
    l0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l0->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    QFont f0 = l0->font();
    f0.setPointSize(7);
    l0->setFont(f0);

    l0->setText(device.serial);

    QLabel *l2 = new QLabel();
    l2->setGeometry(QRect(0, 0, l2_width, height));
    l2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l2->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    QFont f2 = l2->font();
    f2.setPointSize(6);
    l2->setFont(f2);


    QString txt = device.partitionLabels.join('\n');
    l2->setText(txt);

    lay->addLayout(lay2);
    lay2->addWidget(l1);
    lay2->addWidget(l0);
    lay->addWidget(l2);

    DeviceWidget* w = new DeviceWidget();

    w->_usbDevicePath = device.usbDevicePath;
    w->_outputFileName = device.outputFileName;
    w->_serial = device.serial;
    w->setLayout( lay );

    w->setGeometry(QRect(0, 0, width, height));

    return w;
}

void MainWindow::on_pushButton_read_clicked()
{
    emit ReadActionTriggered(this);
}


void MainWindow::on_pushButton_write_clicked()
{
    emit WriteActionTriggered(this);
}

MainViewModel::StringModel MainWindow::get_InputFileName()
{
    MainViewModel::StringModel m;
    QListWidgetItem *item = ui->listWidget_images->selectedItems().first();
    if(item){
        m.txt = item->text();
    }
    return m;
}


