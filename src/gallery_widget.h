#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>

#define MIN_THUMBNAIL_SIZE 150
#define MAX_THUMBNAIL_SIZE 400

class GalleryWidget : public QWidget
{
Q_OBJECT
public:
    explicit GalleryWidget(QWidget *parent = nullptr);
public slots:
    void scroll_value_changed(int value);
private:
    QSlider *m_slider;
    QListWidget *m_list;
};

