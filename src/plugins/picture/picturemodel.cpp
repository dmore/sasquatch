#include "picturemodel.h"

#include <QFile>

PictureModel::PictureModel(QObject *parent)
    : MediaModel(MediaModel::Picture, parent)
{
    QHash<int, QByteArray> roleNames = MediaModel::roleNames();
    setRoleNames(roleNames);
}

PictureModel::~PictureModel()
{
}

QVariant PictureModel::data(MediaInfo *mediaInfo, int role) const
{
    Q_UNUSED(mediaInfo);
    Q_UNUSED(role);
    return QVariant(); // we have nothing special to offer
}

QImage PictureModel::decoration(MediaInfo *info) const
{
    QImage thumbnail = (static_cast<PictureInfo *>(info))->thumbnail;
    if (thumbnail.isNull())
        return QImage(themeResourcePath() + "/media/Fanart_Fallback_Picture_Small.jpg"); // FIXME: Make this configurable
    return thumbnail;
}

MediaInfo *PictureModel::readMediaInfo(const QString &filePath)
{
    QImage image(filePath);
    
    PictureInfo *info = new PictureInfo;
    info->thumbnail = image;

    return info;
}
