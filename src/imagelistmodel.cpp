#include "imagelistmodel.h"

#include <QDebug>
#include <QDir>
#include <QtCore/QDirIterator>

ImageListModel::ImageListModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    imageNameFilter << "*.png"
                    << "*.jpg"
                    << "*.gif";
}

bool ImageListModel::loadDirectoryImageList(const QString& fullPath)
{
    beginResetModel();
    imageFileInfoList.clear();
    QDirIterator it(fullPath, imageNameFilter, QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        imageFileInfoList.push_back(it.fileInfo());
    }
    endResetModel();
    return true;
}

int ImageListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : imageFileInfoList.size();
}

int ImageListModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant ImageListModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid()) {
        if (role == Qt::DisplayRole) {
            return imageFileInfoList[index.row()].absoluteFilePath();
        }
    }
    return QVariant();
}
