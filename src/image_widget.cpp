//
// Created by anton on 24.01.2020.
//

#include <QtWidgets/QVBoxLayout>
#include <utility>
#include "image_widget.h"

ImageWidget::ImageWidget(QPixmap img, QString label, QWidget *parent)
        : QWidget(parent)
{
    img_label = new QLabel(this);
    img_label->setBackgroundRole(QPalette::Base);
    img_label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    img_label->setScaledContents(true);
    img_label->setPixmap(img);

    chkbox = new QCheckBox(this);
    connect(chkbox, &QCheckBox::stateChanged, this, &ImageWidget::checkbox_changes_state);

    neme_label = new QLabel(label, this);

    auto *lt = new QVBoxLayout(this);
    lt->addWidget(img_label);
    auto *ltname = new QHBoxLayout();
    ltname->addStretch(1);
    ltname->addWidget(chkbox, 0);
    ltname->addWidget(neme_label, 1);
    ltname->addStretch(1);
    lt->addLayout(ltname);
    setLayout(lt);
}

void ImageWidget::checkbox_changes_state(int state)
{
    emit check_state_changed(static_cast<Qt::CheckState>(state));
}

QListWidgetItem *ImageWidget::build_listwidget_item(QPixmap img, QString label, QListWidget *listwidget)
{
    auto *item = new QListWidgetItem(listwidget);
    listwidget->addItem(item);
    auto w = new ImageWidget(std::move(img), std::move(label), listwidget);
    listwidget->setItemWidget(item, w);
    item->setSizeHint( w->sizeHint() );
    return item;
}
