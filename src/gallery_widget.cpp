//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtCore/QCoreApplication>
#include "gallery_widget.h"
#include <defines.h>

GalleryWidget::GalleryWidget(QWidget *parent)
    : QWidget(parent)
{
    line_dir_path = new QLineEdit(this);
    line_dir_path->setReadOnly(true);

    btn_browse = new QPushButton("Browse...",this);
    connect(btn_browse, &QPushButton::clicked, this, &GalleryWidget::browse_directory);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setMinimum(MIN_THUMBNAIL_SIZE);
    m_slider->setMaximum(MAX_THUMBNAIL_SIZE);
    m_slider->setValue(MIN_THUMBNAIL_SIZE);
    connect(m_slider, &QSlider::valueChanged, this, &GalleryWidget::scroll_value_changed);

    m_model = new ImageListModel(this);
    m_list = new ImageListView(this);
    m_list->setModel(m_model);
    //m_list->setThumbnailSize(MIN_THUMBNAIL_SIZE);

    auto *mainlt = new QVBoxLayout(this);
    auto *browselt = new QHBoxLayout;
    browselt->addWidget(line_dir_path, 1);
    browselt->addWidget(btn_browse);
    auto *sliderlt = new QHBoxLayout;
    sliderlt->addWidget(new QLabel("Thumbnail size:", this));
    sliderlt->addWidget(m_slider);
    mainlt->addLayout(browselt);
    mainlt->addLayout(sliderlt);
    mainlt->addWidget(m_list);
}

void GalleryWidget::scroll_value_changed(int value)
{
    m_list->setThumbnailSize(value);
}

void GalleryWidget::browse_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if (!dir.isEmpty())
    {
        m_model->loadDirectoryImageList(dir);
        line_dir_path->setText(dir);
    }
}
