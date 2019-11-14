#include "mainwindow.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QScreen>
#include <QtGlobal>
#include <formatter.hpp>
#include <iostream>
#include <ui_mainwindow.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <QListWidget>
#include <QTranslator>
#include <notifications.hpp>
#include <worker/worker.hpp>

bool verbose = false;

// I've experiments to run
// There is research to be done
// On the people who are
// still alive
bool stillAlive = true;

void loadTranslation(QString locale) {
    QFile resource(":/translations/" + locale + ".qm");
    if (!resource.exists()) return;
    resource.open(QIODevice::ReadOnly);

    QTranslator *translator = new QTranslator;
    QByteArray file = resource.readAll();
    QByteArray *permFile = new QByteArray;
    permFile->swap(file);
    translator->load((const unsigned char *)permFile->constData(), permFile->size());
    QApplication::installTranslator(translator);
}

int main(int argc, char *argv[]) {
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationName("KShare");
    a.setOrganizationName("ArsenArsen");
    a.setApplicationVersion("4.1");

    QString locale = QLocale::system().name();
    if (locale != "en_US") loadTranslation(locale);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption h({ "b", "background" }, "Does not show the main window, starts in tray.");
    QCommandLineOption v({ "v", "verbose" }, "Enables QtDebugMsg outputs");
    QCommandLineOption ver({ "ver", "version" }, "Prints KShare version");
    parser.addOption(h);
    parser.addOption(v);
    parser.addOption(ver);
    parser.process(a);

    if (parser.isSet(ver)) {
        printf("%s %s\n", a.applicationName().toLocal8Bit().constData(), a.applicationVersion().toLocal8Bit().constData());
        return 0;
    }

    verbose = parser.isSet(v);
    MainWindow w;
    Worker::init();
    a.connect(&a, &QApplication::aboutToQuit, Worker::end);
    a.connect(&a, &QApplication::aboutToQuit, [] { stillAlive = false; });

    if (!parser.isSet(h)) w.show();
    return a.exec();
}
