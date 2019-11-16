#include "mainwindow.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QScreen>
#include <QtGlobal>
#include <formatter.hpp>
#include <iostream>
#include <ui_mainwindow.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <QListWidget>
#include <QTranslator>
#include <notifications.hpp>
#include <worker/worker.hpp>

// I've experiments to run
// There is research to be done
// On the people who are
// still alive
bool stillAlive = true;

int main(int argc, char* argv[])
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
    srand((unsigned int)time(nullptr));

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationName("KShare");
    a.setOrganizationName("ArsenArsen");
    a.setApplicationVersion("4.1");

    QString locale = QLocale::system().name();
    QTranslator translator;
    if (translator.load(locale, ":/translations/"))
        a.installTranslator(&translator);

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption bg({ "b", "background" }, "Does not show the main window, starts in tray.");
    QCommandLineOption ver({ "v", "version" }, "Prints KShare version");
    parser.addOption(bg);
    parser.addOption(ver);
    parser.process(a);

    if (parser.isSet(ver))
    {
        printf("%s %s\n", a.applicationName().toLocal8Bit().constData(), a.applicationVersion().toLocal8Bit().constData());
        return 0;
    }

    MainWindow w;
    Worker::init();
    a.connect(&a, &QApplication::aboutToQuit, Worker::end);
    a.connect(&a, &QApplication::aboutToQuit, [] { stillAlive = false; });

    if (!parser.isSet(bg))
        w.show();
    return a.exec();
}
