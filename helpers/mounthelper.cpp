#include "mounthelper.h"
#include "processhelper.h"

extern ProcessHelper _processHelper;

bool MountHelper::Mount(const QString& devpath, const QString& mountpath){
    if(devpath.isEmpty()) return false;
    if(mountpath.isEmpty()) return false;

    QString cmd = QStringLiteral(R"(mount %1 %3)").arg(devpath,mountpath);
    auto out = _processHelper.ShellExecuteSudo(cmd);

    if(out.exitCode) return false;
    if(!out.stdErr.isEmpty()) return false;

    return true;
}

bool MountHelper::UMount(const QString& mountpath){
    if(mountpath.isEmpty()) return false;
    QString cmd = QStringLiteral(R"(umount %1)").arg(mountpath);
    auto out = _processHelper.ShellExecuteSudo(cmd);

    if(out.exitCode) return false;
    if(!out.stdErr.isEmpty()) return false;

    return true;
}

QString MountHelper::MkMountPoint(const QString& mountdir)
{
    if(mountdir.isEmpty()) return {};
    QString mountPoint = QStringLiteral("/mnt/%1").arg(mountdir);

    QString cmd = QStringLiteral("mkdir -p %1").arg(mountPoint);

    _processHelper.ShellExecuteSudo(cmd);

    return mountPoint;
}

QString MountHelper::GetMountPoint(const QString &dev)
{
    //QString cmd = QStringLiteral(R"(df -l --output=target %1)").arg(dev);
    QString cmd = QStringLiteral(R"(findmnt -r --output=target %1)").arg(dev);
    auto out = _processHelper.ShellExecute(cmd);

    if(out.exitCode!=0) return {};
    if(out.stdOut.isEmpty()) return {};
    auto a = out.stdOut.split('\n');
    if(a.size()<2) return {};
    if(a[1].isEmpty()) return {};

    return a[1];
}

