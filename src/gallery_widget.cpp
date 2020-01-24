//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include "gallery_widget.h"
#include "image_widget.h"

GalleryWidget::GalleryWidget(QWidget *parent)
    : QWidget(parent)
{
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setMinimum(MIN_THUMBNAIL_SIZE);
    m_slider->setMaximum(MAX_THUMBNAIL_SIZE);
    m_slider->setValue(MIN_THUMBNAIL_SIZE);
    connect(m_slider, &QSlider::valueChanged, this, &GalleryWidget::scroll_value_changed);

    m_list = new QListWidget(this);
    m_list->setViewMode(QListView::IconMode);
    //m_list->setIconSize(QSize(200,200));
    m_list->setResizeMode(QListWidget::Adjust);
    m_list->setDragEnabled(false);
    m_list->setGridSize(QSize(MIN_THUMBNAIL_SIZE, MIN_THUMBNAIL_SIZE));
    ImageWidget::build_listwidget_item(QImage("/home/anton/Downloads/img/1.jpg"),"Earth", m_list);
    ImageWidget::build_listwidget_item(QImage("/home/anton/Downloads/img/2.jpg"),"Tornado", m_list);
    ImageWidget::build_listwidget_item(QImage("/home/anton/Downloads/img/3.jpg"),"Wallpaper", m_list);

    auto *mainlt = new QVBoxLayout(this);
    auto *sliderlt = new QHBoxLayout;
    sliderlt->addWidget(new QLabel("Thumbnail size:", this));
    sliderlt->addWidget(m_slider);
    mainlt->addLayout(sliderlt);
    mainlt->addWidget(m_list);
}

void GalleryWidget::scroll_value_changed(int value)
{
    m_list->setGridSize(QSize(value,value));
}
