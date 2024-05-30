#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QFileInfoList>
#include <QString>


class FileHelper
{
public:
    static bool Exists(const QString &filename);
    static QStringList GetFiles(const QString &dir, const QStringList& filter);
    static QFileInfoList GetSystemFiles(const QString &dir, const QStringList& filter);

    static QStringList GetRootList(const QFileInfoList& files, const QString& filter);
};

#endif // FILEHELPER_H
