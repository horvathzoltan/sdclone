#include <QApplication>
#include <QtCharts>

#include <helpers/processhelper.h>

#include "mainwindow.h"
#include "mainpresenter.h"

ProcessHelper _processHelper;

auto main(int argc, char *argv[]) -> int
{   
    QApplication a(argc, argv);

    _processHelper.SetPassword("Aladar123");
    _processHelper.setWriteErr(false);
    MainWindow w;
    MainPresenter p;
    p.appendView(&w);
    w.show();    
    p.initView(&w);

    return a.exec();
}
