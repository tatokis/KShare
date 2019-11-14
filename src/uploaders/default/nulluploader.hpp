#ifndef NULLUPLOADER_HPP
#define NULLUPLOADER_HPP

#include <QApplication>
#include <uploaders/uploader.hpp>

class NullUploader : public Uploader {
    Q_DECLARE_TR_FUNCTIONS(NullUploader)
public:
    QString name() {
        return "null";
    }
    QString description() {
        return "Does absolutely nothing. Useful for just saving images.";
    }

    void doUpload(QByteArray imgData, QString format);
};

#endif // NULLUPLOADER_HPP
