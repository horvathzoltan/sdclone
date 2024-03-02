#include "filehelper.h"

#include <QDir>
#include <QFileInfo>

bool FileHelper::Exists(const QString &filename)
{
    if(filename.isEmpty()) return false;

    QFileInfo fi(filename);
    if(!fi.isAbsolute())
    {
        return false;
    }

    if(!fi.exists())
    {
        return false;
    }

    return true;
}

QStringList FileHelper::GetFiles(const QString &dir, const QStringList& filter)
{
    if(dir.isEmpty()) return {};
    QDir d(dir);
    if(!d.exists()) return {};
    QStringList e = d.entryList(filter, QDir::Files);
    return e;
}

