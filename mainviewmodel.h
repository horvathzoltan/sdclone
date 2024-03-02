#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QtGlobal>
#include <QList>
#include <QColor>

class MainViewModel
{
public:

    struct DoWorkModel{
        int i;
    };

    struct DoWorkRModel{
        QString txt;
    };

    struct StringModel{
        QString txt;
    };

    struct StringListModel{
        QStringList txts;
    };
};


#endif // MAINVIEWMODEL_H
