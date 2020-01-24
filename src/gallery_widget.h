#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>
#include "imagelistview.h"
#include "imagelistmodel.h"

class GalleryWidget : public QWidget
{
Q_OBJECT
public:
    explicit GalleryWidget(QWidget *parent = nullptr);
private slots:
    void scroll_value_changed(int value);
    void browse_directory();
private:
    QLineEdit *line_dir_path;
    QPushButton *btn_browse;
    QSlider *m_slider;
    ImageListView *m_list;
    ImageListModel *m_model;
};

