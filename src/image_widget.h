#pragma once

#include <QtWidgets/QListWidgetItem>

class ImageWidget : public QWidget
{
public:
    ImageWidget(QImage img, QString label, QWidget *parent = nullptr);

    static QListWidgetItem* build_listwidget_item(QImage img, QString label, QListWidget *listwidget = nullptr);
};
