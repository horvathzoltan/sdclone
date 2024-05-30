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

QFileInfoList FileHelper::GetSystemFiles(const QString &dir, const QStringList& filter)
{
    if(dir.isEmpty()) return {};
    QDir d(dir);
    if(!d.exists()) return {};
    QFileInfoList e = d.entryInfoList(filter,QDir::System);
    return e;
}

QStringList FileHelper::GetRootList(const QFileInfoList& files, const QString& filter){
    QStringList dev;
    for(auto&a:files){
        QString fileName = a.fileName();
        if(!fileName.contains(filter)) continue;
        auto target = a.symLinkTarget();

        int L = target.length();
        QChar l = target.at(L-1);
        if(l.isDigit()) continue;
        dev.append(target);
    }
    return dev;
}
