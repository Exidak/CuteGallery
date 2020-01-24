#include <QApplication>
#include <QtWidgets/QMainWindow>
#include "gallery_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    GalleryWidget wdg(&w);
    w.setMinimumSize(QSize(800, 600));
    w.setCentralWidget(&wdg);
    w.show();

    return QApplication::exec();
}
