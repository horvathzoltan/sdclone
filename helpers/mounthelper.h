#ifndef MOUNTHELPER_H
#define MOUNTHELPER_H

#include <QString>


class MountHelper
{
public:
    //static bool isMounted(const QString& localPath);
    static bool Mount(const QString& devpath, const QString& mountpath);
    static bool UMount(const QString& mountpath);
    static QString MkMountPoint(const QString& dir);
    static QString GetMountPoint(const QString& dev);
};


#endif // MOUNTHELPER_H
