#pragma once

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QList>

class ImageListModel : public QAbstractTableModel {
    Q_OBJECT
public:
    ImageListModel(QObject* parent = Q_NULLPTR);
public:
    bool loadDirectoryImageList(const QString& fullPath);
public:
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;

private:
    QStringList imageNameFilter; // filters list
    QFileInfoList imageFileInfoList; // files list
};
