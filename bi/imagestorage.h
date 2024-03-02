#ifndef IMAGESTORAGE_H
#define IMAGESTORAGE_H

#include <QString>
#include <QStringList>

#include <helpers/filenamehelper.h>


class ImageStorage
{
public:
    ImageStorage();
    void Init(const QString& partLabel);

    enum Error:int{OK=0,NoDevPath,CannotMount};
private:
    QString _mountPoint;
    QString _devPath;
    Error _err;
    QStringList _imageFilePaths;
public:
    QString devPath(){return _devPath;}
    bool isMounted(){ return !_mountPoint.isEmpty();}
    Error err(){return _err;}
    QStringList imageFilePaths(){return _imageFilePaths;}

    QString mountPoint(){return _mountPoint;}
    QString GetFolder(const QString& dir){
        return FileNameHelper::Combine(_mountPoint, dir);
    }

    QStringList GetImageFilePaths(const QString& imageFolder);
};

#endif // IMAGESTORAGE_H
