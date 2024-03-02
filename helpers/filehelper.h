#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QString>


class FileHelper
{
public:
    static bool Exists(const QString &filename);
    static QStringList GetFiles(const QString &dir, const QStringList& filter);
};

#endif // FILEHELPER_H
