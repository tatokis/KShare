#include "nulluploader.hpp"
#include "notifications.hpp"
void NullUploader::doUpload(QByteArray imgData, QString format) {
    notifications::notify(QApplication::applicationDisplayName(), tr("Screenshot taken"));
}
