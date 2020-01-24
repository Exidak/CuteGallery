//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include <utility>
#include <QtWidgets/QLabel>
#include "image_widget.h"

ImageWidget::ImageWidget(QImage img, QString label, QWidget *parent)
        : QWidget(parent)
{
    auto *lt = new QVBoxLayout(this);
    lt->addWidget(new QLabel("blah", this));
    lt->addWidget(new QLabel("blah1", this));
    lt->addWidget(new QLabel("blah2", this));
    setLayout(lt);
    printf("Init\n");
}

QListWidgetItem *ImageWidget::build_listwidget_item(QImage img, QString label, QListWidget *listwidget)
{
    auto *item = new QListWidgetItem(listwidget) ;
    listwidget->addItem(item);
    auto w = new ImageWidget(std::move(img), std::move(label), listwidget);
    listwidget->setItemWidget(item, w);
    item->setSizeHint( w->sizeHint() );
    return item;
}
