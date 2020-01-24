//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QCoreApplication>
#include "gallery_widget.h"
#include "image_widget.h"

GalleryWidget::GalleryWidget(QWidget *parent)
    : QWidget(parent)
{
    scaner = new ImageScaner;
    connect(scaner, &ImageScaner::image_found, this, &GalleryWidget::found_image);
    connect(scaner, &ImageScaner::finished, this, &GalleryWidget::scaner_finished);

    line_dir_path = new QLineEdit(this);
    line_dir_path->setReadOnly(true);

    btn_browse = new QPushButton("Browse...",this);
    connect(btn_browse, &QPushButton::clicked, this, &GalleryWidget::browse_directory);

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
    m_list->setGridSize(QSize(value,value));
}

void GalleryWidget::browse_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if (!dir.isEmpty())
    {
        m_list->clear();
        btn_browse->setEnabled(false);
        line_dir_path->setText(dir);
        scaner->set_dir(dir);
        scaner->start();
    }
}

void GalleryWidget::found_image(QFileInfo fi)
{
    ImageWidget::build_listwidget_item(QPixmap(fi.absoluteFilePath()),fi.fileName(), m_list);
    m_list->update();
    QCoreApplication::processEvents();
}

void GalleryWidget::scaner_finished()
{
    btn_browse->setEnabled(true);
    QCoreApplication::processEvents();
}
