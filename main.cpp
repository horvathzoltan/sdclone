#include <QApplication>
#include <QtCharts>

#include "mainwindow.h"
#include "mainpresenter.h"

// gitteszt 1

auto main(int argc, char *argv[]) -> int
{   
    QApplication a(argc, argv);

    MainWindow w;
    MainPresenter p;
    p.appendView(&w);
    w.show();    
    p.initView(&w);

    return a.exec();
}
