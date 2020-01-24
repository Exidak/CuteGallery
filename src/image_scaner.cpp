//
// Created by anton on 24.01.2020.
//

#include <QtCore/QDirIterator>
#include "image_scaner.h"

void ImageScaner::run(void)
{
    QDirIterator it(_dir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        if (it.fileInfo().suffix() == "jpg")
        {
            emit image_found(it.fileInfo());
        }
    }

}

void ImageScaner::set_dir(QString dir)
{
    _dir = dir;
}
