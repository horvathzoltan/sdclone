#ifndef BLOCKDEVICEHELPER_H
#define BLOCKDEVICEHELPER_H

#include <QString>


class BlockDeviceHelper
{
public:
    static QString GetByLabel(const QString& label);
};

#endif // BLOCKDEVICEHELPER_H
