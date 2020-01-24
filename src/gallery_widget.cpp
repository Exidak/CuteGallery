//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include "gallery_widget.h"

GalleryWidget::GalleryWidget(QWidget *parent)
    : QWidget(parent)
{
    m_list = new QListWidget;
    m_list->setViewMode(QListView::IconMode);
    m_list->setIconSize(QSize(200,200));
    m_list->setResizeMode(QListWidget::Adjust);
    m_list->setGridSize(QSize(104, 104));
    m_list->addItem(new QListWidgetItem(QIcon("/home/anton/Downloads/img/1.jpg"),"Earth"));
    m_list->addItem(new QListWidgetItem(QIcon("/home/anton/Downloads/img/2.jpg"),"Tornado"));
    m_list->addItem(new QListWidgetItem(QIcon("/home/anton/Downloads/img/3.jpg"),"Wallpaper"));

    auto *windowLayout = new QVBoxLayout(this);
    windowLayout->addWidget(m_list);
}
