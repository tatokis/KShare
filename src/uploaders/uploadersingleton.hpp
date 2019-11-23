#ifndef UPLOADERSINGLETON_HPP
#define UPLOADERSINGLETON_HPP

#include "uploader.hpp"
#include <QDir>
#include <QMap>

class UploaderSingleton : public QObject
{
    Q_OBJECT
public:
    static UploaderSingleton& inst()
    {
        static UploaderSingleton inst;
        return inst;
    }
    void registerUploader(Uploader* uploader);
    void upload(QPixmap pixmap);
    void upload(QByteArray img, QString format);
    void upload(QFile& img, QString format);
    void showSettings();
    QList<Uploader*> uploaderList();
    void set(QString uploader);
    QString selectedUploader();
    QList<std::runtime_error> errors();
    QString currentUploader();
    bool validate();

signals:
    void newUploader(Uploader* u);
    void uploaderChanged(QString newName);

private:
    void updateSaveSettings();
    QDir saveDir;
    bool saveImages = true;
    QMap<QString, Uploader*> uploaders;
    QString uploader = "imgur";
    QList<std::runtime_error> errs;
    UploaderSingleton();
    ~UploaderSingleton();

};

#endif // UPLOADERSINGLETON_HPP
