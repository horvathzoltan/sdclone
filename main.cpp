#include <QApplication>
#include <QtCharts>

#include <helpers/processhelper.h>

#include "mainwindow.h"
#include "mainpresenter.h"

auto main(int argc, char *argv[]) -> int
{   
    QApplication a(argc, argv);

    ProcessHelper::SetPassword("Aladar123");

    MainWindow w;
    MainPresenter p;
    p.appendView(&w);
    w.show();    
    p.initView(&w);

    return a.exec();
}
