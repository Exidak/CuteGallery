#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>

class GalleryWidget : public QWidget
{
Q_OBJECT
public:
    explicit GalleryWidget(QWidget *parent = nullptr);
private:
    QListWidget *m_list{};
};

