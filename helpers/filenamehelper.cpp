#include "filenamehelper.h"

#include <QDir>



QString FileNameHelper::Combine(const QString &path1, const QString &path2)
{
    QDir d(path1);
    return d.filePath(path2);
}




