#include "mainwindow.hpp"
#include "aboutbox.hpp"
#include "screenshotter.hpp"
#include "settingsdialog.hpp"
#include "ui_mainwindow.h"
#include "utils.hpp"
#include <QMessageBox>
#include <QShortcut>
#include <colorpicker/colorpickerscene.hpp>
#include <formats.hpp>
#include <hotkeying.hpp>
#include <logger.hpp>
#include <logs/historydialog.hpp>
#include <platformbackend.hpp>
#include <recording/recordingformats.hpp>
#include <settings.hpp>
#include <uploaders/uploadersingleton.hpp>

MainWindow* MainWindow::instance;

void MainWindow::rec()
{
    if (controller->isRunning())
        return;
    auto f = static_cast<formats::Recording>(
    settings::settings().value("recording/format", static_cast<int>(formats::Recording::None)).toInt());
    if (f >= formats::Recording::None)
    {
        logger::warn(tr("Recording format not set in settings. Aborting."));
        return;
    }
    RecordingContext* ctx = new RecordingContext;
    RecordingFormats* format = new RecordingFormats(f);
    ctx->consumer = format->getConsumer();
    ctx->finalizer = format->getFinalizer();
    ctx->validator = format->getValidator();
    ctx->format = format->getFormat();
    ctx->postUploadTask = format->getPostUploadTask();
    ctx->anotherFormat = format->getAnotherFormat();
    controller->start(ctx);
}

#define ACTION(english, menu)                                                                                          \
    [&]() -> QAction* {                                                                                                \
        QAction* a = menu->addAction(english);                                                                         \
        return a;                                                                                                      \
    }()

void addHotkey(QString name, std::function<void()> action)
{
    hotkeying::load(name, action);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    instance = this;
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/icon.png"));
    tray = new QSystemTrayIcon(windowIcon(), this);
    tray->setToolTip(QApplication::applicationName());
    tray->setVisible(true);
    QMenu* menu = new QMenu(this);
    QAction* shwnd = ACTION(tr("Show Window"), menu);
    QAction* fullscreen = ACTION(tr("Desktop"), menu);
    QAction* area = ACTION(tr("Selection"), menu);

#ifdef PLATFORM_CAPABILITY_ACTIVEWINDOW
    QAction* active = ACTION(tr("Active window"), menu);
    connect(active, &QAction::triggered, this, [] { screenshotter::activeDelayed(); });
#endif
    QAction* picker = ACTION(tr("Color picker"), menu);
    QAction* rec = ACTION(tr("Record screen"), menu);
    QAction* recoff = ACTION(tr("Stop recording"), menu);
    QAction* recabort = ACTION(tr("Abort recording"), menu);
    QAction* quit = ACTION(tr("Quit"), menu);

    menu->addAction(shwnd);
    menu->addSeparator();
    menu->addAction(picker);
    menu->addSeparator();
    menu->addActions({ area, fullscreen });
#ifdef PLATFORM_CAPABILITY_ACTIVEWINDOW
    menu->addAction(active);
#endif
    menu->addSeparator();
    menu->addActions({ rec, recoff, recabort });
    menu->addSeparator();
    menu->addAction(quit);
    connect(quit, &QAction::triggered, this, &MainWindow::quit);
    connect(shwnd, &QAction::triggered, this, &QMainWindow::show);
    connect(picker, &QAction::triggered, [] { ColorPickerScene::showPicker(); });
    connect(tray, &QSystemTrayIcon::messageClicked, this, &QMainWindow::show);
    connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick)
            show();
    });
    connect(fullscreen, &QAction::triggered, this, [] { screenshotter::fullscreenDelayed(); });
    connect(area, &QAction::triggered, this, [] { screenshotter::areaDelayed(); });
    connect(rec, &QAction::triggered, this, &MainWindow::rec);
    connect(recoff, &QAction::triggered, controller, &RecordingController::end);
    connect(recabort, &QAction::triggered, controller, &RecordingController::abort);
    connect(ui->settings, &QPushButton::clicked, this, &MainWindow::on_actionSettings_triggered);

    tray->setContextMenu(menu);

    addHotkey("fullscreen", [] { screenshotter::fullscreen(); });
    addHotkey("area", [] { screenshotter::area(); });
    addHotkey("active", [] { screenshotter::active(); });
    addHotkey("picker", [] { ColorPickerScene::showPicker(); });
    addHotkey("recordingstop", [&] { controller->end(); });
    addHotkey("recordingabort", [&] { controller->abort(); });
    addHotkey("recordingstart", [&] { this->rec(); });

    auto errors = UploaderSingleton::inst().errors();
    for (auto err : errors)
        ui->logBox->addItem(QString("ERROR: ") + err.what());
    setWindowTitle(QApplication::applicationName());
    val = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::valid()
{
    return val;
}

MainWindow* MainWindow::inst()
{
    return instance;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (settings::settings().value("hideOnClose", true).toBool())
    {
        event->ignore();
        hide();
    }
    else
        QApplication::exit(0);
}

void MainWindow::quit()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionQuit_triggered()
{
    quit();
}

void MainWindow::on_actionFullscreen_triggered()
{
    screenshotter::fullscreenDelayed();
}

void MainWindow::on_actionArea_triggered()
{
    screenshotter::areaDelayed();
}

void MainWindow::on_actionStart_triggered()
{
    rec();
}

void MainWindow::on_actionStop_triggered()
{
    controller->end();
}

void MainWindow::on_actionColor_Picker_triggered()
{
    ColorPickerScene::showPicker();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog* dialog = new SettingsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutBox* box = new AboutBox(this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void MainWindow::on_actionActive_window_triggered()
{
    screenshotter::activeDelayed();
}

void MainWindow::on_actionAbort_triggered()
{
    controller->abort();
}

void MainWindow::on_history_clicked()
{
    HistoryDialog* dialog = new HistoryDialog;
    dialog->show();
}

void MainWindow::setTrayIcon(QIcon icon)
{
    tray->setIcon(icon);
}
