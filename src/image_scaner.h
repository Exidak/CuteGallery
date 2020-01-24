#pragma once

#include <QtCore/QThread>
#include <QtCore/QFileInfo>

class ImageScaner : public QThread
{
Q_OBJECT
public:
    void run( void ) override;

    void set_dir(QString dir);
signals:
    void image_found( QFileInfo );
private:
    QString _dir;
};