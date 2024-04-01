#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "devicewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit_status->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->label_progress->setVisible(false);
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
    if(m.txt.isEmpty()) return;

    QString u;
    for(auto&a:m.txt){
        if(a == '\r'){
            QTextCursor cursor = ui->plainTextEdit_status->textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.select(QTextCursor::LineUnderCursor);
            cursor.removeSelectedText();
            cursor.deletePreviousChar(); // Added to trim the newline char when removing last line
            ui->plainTextEdit_status->setTextCursor(cursor);
        } else if (a == '\n') {
            if(!u.isEmpty()){
                ui->plainTextEdit_status->appendPlainText(u);//a.join('\n'));//setPlainText(a.join('\n'));
                u.clear();
            }
        }
        else{
            u.append(a);
        }
    }
    if(!u.isEmpty()){
        ui->plainTextEdit_status->appendPlainText(u);
    }
}

/*
processWriteAction
check:/dev/sdc:1-1.1.2_1.0:OK
ok
finished
*/
MainViewModel::WriteStatusWM MainWindow::getLastWriteStatus(){
    QString a = ui->plainTextEdit_status->toPlainText();
    MainViewModel::WriteStatusWM r;

    int ix = a.lastIndexOf("processWriteAction");
    if(ix>0){
        int ix2 = a.indexOf("finished", ix);
        if(ix2>0){
            QStringList b = a.mid(ix, ix2-ix).split('\n');
            for(auto&c:b){
                if(c.startsWith("check:")){
                    QStringList tokens = c.split(':');
                    if(tokens.length()>=4){
                        MainViewModel::WriteStatus m;
                        m.devicePath = tokens[1];
                        m.usbPath = tokens[2];
                        m.status = tokens[3]=="OK";
                        r.append(m);
                    }
                }
            }
        }
    }
    return r;
}

MainViewModel::IntModel MainWindow::get_WriteBytes()
{
    QString a = ui->plainTextEdit_status->toPlainText();
    MainViewModel::IntModel r{0};

    int ix = a.lastIndexOf("processWriteAction");
    if(ix>0){
        int ix2 = a.indexOf("finished", ix);
        if(ix2>0){
            QStringList b = a.mid(ix, ix2-ix).split('\n');
            for(auto&c:b){
                if(c.startsWith("writing:")){
                    QStringList tokens = c.split(' ');
                    if(tokens.length()>=4){
                        bool ok;
                        qlonglong i = tokens[1].toLongLong(&ok);
                        if(ok){
                            r.value = i;
                            break;
                        }
                    }
                }
            }
        }
    }
    return r;
}


void MainWindow::set_PresenterStatus(const MainViewModel::StringModel &m)
{
    ui->label_status->setText(m.txt);
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

void MainWindow::set_DeviceWriteStates(const MainViewModel::WriteStatusWM& m) {
    for(auto&a:m.states()){
        SetDeviceWriteState(a);
    }
}

void MainWindow::SetDeviceWriteState(const MainViewModel::WriteStatus &m)
{
    for(int row = 0; row < ui->listWidget_devices->count(); row++){
        QListWidgetItem *item = ui->listWidget_devices->item(row);
        DeviceWidget *w = (DeviceWidget*)(ui->listWidget_devices->itemWidget(item));
        if(w){
            if(w->_usbDevicePath==m.usbPath){
                w->setStatus(m.status);
            }
        }
    }
}

void MainWindow::set_ClearDeviceWriteStates() {
    for(int row = 0; row < ui->listWidget_devices->count(); row++){
        QListWidgetItem *item = ui->listWidget_devices->item(row);
        DeviceWidget *w = (DeviceWidget*)(ui->listWidget_devices->itemWidget(item));
        if(w) w->resetStatus();
    }
}


void MainWindow::set_DeviceList(const MainViewModel::DeviceListModel &m)
{
    static bool lock = false;
    if(lock) return;
    lock = true;
    //set_StatusLine({"set_DeviceList"});
    //QList<QListWidgetItem*> itemsToDelete;
    QList<int> rowToDelete;

    for(int row = 0; row < ui->listWidget_devices->count(); row++){
        QListWidgetItem *item = ui->listWidget_devices->item(row);
        DeviceWidget *w = (DeviceWidget*)(ui->listWidget_devices->itemWidget(item));        
        if(w){
            bool contains = m.containsBySerial(w->_serial);

            if(!contains){
                //itemsToDelete.append(item);
                set_StatusLine({"remove device:"+w->_usbDevicePath});
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

            set_StatusLine({"add device:"+device.usbDevicePath});
        }
    }
    int a = ui->listWidget_devices->count();
    //set_StatusLine({"items:"+QString::number(a)});
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
    int width = s.width()-16;
    int height = 45;
    int l1_width = s.width()/3;
    int l2_width = s.width()-l1_width;

    QHBoxLayout *lay= new QHBoxLayout();
    lay->setGeometry(QRect(0, 0, width, height));
    lay->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    lay->setSpacing(0);
    lay->setMargin(0);
    QVBoxLayout *lay2= new QVBoxLayout();
    lay2->setGeometry(QRect(0, 0, width, height));
    lay2->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    lay2->setSpacing(0);
    lay2->setMargin(0);
    // device
    QLabel *l1 = new QLabel();
    QFont f1 = l1->font();
    f1.setPointSize(10);
    l1->setFont(f1);
    l1->setText(device.deviceLabel);


    l1->setGeometry(QRect(0, 0, l1_width, (height/3)*2));
    l1->setMinimumSize(l1_width, (height/3)*2);
    l1->setMaximumSize(l1_width, (height/3)*2);

    l1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l1->setAlignment(Qt::AlignTop|Qt::AlignLeft);

    //l1->setMargin(2);

    // serial
    QLabel *l0 = new QLabel();

    l0->setGeometry(QRect(0, 0, l1_width, (height/3)));

    QFont f0 = l0->font();
    f0.setPointSize(7);
    l0->setFont(f0);
    l0->setText(device.serial);//serial

    l0->setMinimumSize(l1_width, (height/3));
    l0->setMaximumSize(l1_width, (height/3));
    //l0->setMargin(2);
    l0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l0->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
//l0->setMargin(2);

    // partitions
    QLabel *l2 = new QLabel();
    l2->setGeometry(QRect(0, 0, l2_width, height));
    l2->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    //l2->setMargin(2);
    QFont f2 = l2->font();
    f2.setPointSize(6);
    l2->setFont(f2);

    QString txt = device.partitionLabels.join('\n');
    l2->setText(txt);

    //QFrame* sep = new QFrame;
    //sep->setFrameShape(QFrame::HLine);

    lay->addLayout(lay2);
    lay2->addWidget(l1);
    lay2->addWidget(l0);
    lay->addWidget(l2);
    //lay->addWidget(sep);

    DeviceWidget* w = new DeviceWidget();
    // QPalette pal = QPalette();

    // // set black background
    // // Qt::black / "#000000" / "black"
    // pal.setColor(QPalette::Window, Qt::gray);

    // w->setAutoFillBackground(true);
    // w->setPalette(pal);

    w->setMinimumSize(width, height);
    w->setMaximumSize(width, height);
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    w->setStatusLabel(l0);
    w->setLabelLabel(l1);
    w->_usbDevicePath = device.usbDevicePath;
    w->_outputFileName = device.outputFileName;
    w->_serial = device.serial;
    w->setLayout( lay );

    w->setGeometry(QRect(0, 0, width, height));

    QPalette pal = QPalette();
    w->setDefaultBackground(Qt::GlobalColor::transparent);

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
    auto items = ui->listWidget_images->selectedItems();
    if(!items.isEmpty()){
        QListWidgetItem *item = items.first();
        if(item){
            m.txt = item->text();
        }
    }
    return m;
}

void MainWindow::on_pushButton_exit_clicked()
{
    emit ExitActionTriggered(this);
}


void MainWindow::on_pushButton_log_clicked()
{
    bool visible = ui->plainTextEdit_status->isVisible();
    ui->plainTextEdit_status->setVisible(!visible);
}

/*progress*/
void MainWindow::set_ShowProgressbar()
{
    ui->progressBar->setVisible(true);
    ui->label_progress->setVisible(true);
}

void MainWindow::set_HideProgressbar()
{
    ui->progressBar->setVisible(false);
    ui->label_progress->setVisible(false);
}

void MainWindow::set_ProgressText(const MainViewModel::StringModel &m)
{
    ui->label_progress->setText(m.txt);
}

void MainWindow::set_ProgressLine(const MainViewModel::IntModel& m){
    ui->progressBar->setValue(m.value);
}
