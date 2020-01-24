#pragma once

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    ImageWidget(QPixmap img, QString label, QWidget *parent = nullptr);

    static QListWidgetItem* build_listwidget_item(QPixmap img, QString label, QListWidget *listwidget = nullptr);
signals:
    void check_state_changed(Qt::CheckState state);
private slots:
    void checkbox_changes_state(int state);
private:
    QLabel *img_label;
    QCheckBox *chkbox;
    QLabel *neme_label;
};
