#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include "image_scaner.h"

#define MIN_THUMBNAIL_SIZE 150
#define MAX_THUMBNAIL_SIZE 400

class GalleryWidget : public QWidget
{
Q_OBJECT
public:
    explicit GalleryWidget(QWidget *parent = nullptr);
private slots:
    void scroll_value_changed(int value);
    void browse_directory();
    void found_image(QFileInfo fi);
    void scaner_finished();
private:
    ImageScaner *scaner;

    QLineEdit *line_dir_path;
    QPushButton *btn_browse;
    QSlider *m_slider;
    QListWidget *m_list;
};

