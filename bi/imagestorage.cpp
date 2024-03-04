#include "imagestorage.h"

#include <helpers/blockdevicehelper.h>
#include <helpers/filehelper.h>
#include <helpers/mounthelper.h>

ImageStorage::ImageStorage() {}

void ImageStorage::Init(const QString& partLabel){
    _devPath = BlockDeviceHelper::GetByLabel(partLabel);
    if(!_devPath.isEmpty())
    {
        _mountPoint = MountHelper::GetMountPoint(_devPath);
        if(!isMounted()){
            _mountPoint = MountHelper::MkMountPoint(partLabel);
            bool mount_ok = MountHelper::Mount(_devPath, _mountPoint);
            if(!mount_ok){
                _err = Error::CannotMount;
            } else{
                _err=Error::OK;
            }
        } else{
            _err=Error::OK;
        }
    }
    else
    {
        _err = Error::NoDevPath;
    }
}

void ImageStorage::SetImageFilePath(const QString& imageFolder){
    _imageFolder = GetFolder(imageFolder);
}

QStringList ImageStorage::GetImageFilePaths(){

    if(_imageFolder.isEmpty()) return {};
    QStringList imageFilePaths = FileHelper::GetFiles(_imageFolder, {"*.img"});
    return imageFilePaths;
}


