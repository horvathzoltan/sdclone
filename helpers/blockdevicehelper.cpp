#include "blockdevicehelper.h"
#include "processhelper.h"

extern ProcessHelper _processHelper;

QString BlockDeviceHelper::GetByLabel(const QString &label)
{
    QString cmd = QStringLiteral(R"(lsblk -ro path,type,label)");
    auto out = _processHelper.ShellExecute(cmd);

    if(out.exitCode!=0) return {};
    if(out.stdOut.isEmpty()) return {};

    QStringList lines = out.stdOut.split('\n');
    for(auto&l:lines)
    {
        if(l.isEmpty()) continue;

        auto words=l.split(' ');
        if(words.count()<3) continue;

        bool found = words[1]=="part" && words[2]==label;

        if(found) return words[0];
    }
    return {};
}
