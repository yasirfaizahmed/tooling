#include "main_window.h"

#include "app_info.h"
#include "panels/inspector/inspector_panel.h"
#include "panels/media_bin/media_bin_panel.h"
#include "panels/panel_title_bar.h"
#include "panels/preview/preview_panel.h"
#include "panels/timeline/timeline_panel.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QGuiApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>

namespace cutline::app {

namespace {

// Bump when the dock layout changes incompatibly; older saved layouts are then ignored.
constexpr int kLayoutVersion = 1;

const QString kGeometryKey = QStringLiteral("mainWindow/geometry");
const QString kStateKey = QStringLiteral("mainWindow/state");

} // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QString::fromLatin1(kAppName));
    setMinimumSize(960, 600);
    if (const QScreen* screen = QGuiApplication::primaryScreen()) {
        resize(screen->availableGeometry().size() * 0.85);
    }

    setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks);
    // The bottom dock area takes both bottom corners, so the timeline spans the full width.
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    previewPanel_ = new PreviewPanel(this);
    setCentralWidget(previewPanel_);

    mediaBinPanel_ = new MediaBinPanel;
    inspectorPanel_ = new InspectorPanel;
    timelinePanel_ = new TimelinePanel;
    mediaBinDock_ = createDock(mediaBinPanel_, tr("Media Bin"), QStringLiteral("mediaBinDock"));
    inspectorDock_ = createDock(inspectorPanel_, tr("Inspector"), QStringLiteral("inspectorDock"));
    timelineDock_ = createDock(timelinePanel_, tr("Timeline"), QStringLiteral("timelineDock"));

    createMenus();
    resetLayout();
}

QDockWidget* MainWindow::createDock(QWidget* panel, const QString& title,
                                    const QString& objectName) {
    auto* dock = new QDockWidget(title, this);
    dock->setObjectName(objectName); // saveState() identifies docks by object name
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |
                      QDockWidget::DockWidgetClosable);
    dock->setTitleBarWidget(new PanelTitleBar(title, dock));
    dock->setWidget(panel);
    return dock;
}

void MainWindow::resetLayout() {
    const struct {
        QDockWidget* dock;
        Qt::DockWidgetArea area;
    } placements[] = {
        {mediaBinDock_, Qt::LeftDockWidgetArea},
        {inspectorDock_, Qt::RightDockWidgetArea},
        {timelineDock_, Qt::BottomDockWidgetArea},
    };
    for (const auto& placement : placements) {
        removeDockWidget(placement.dock);
        addDockWidget(placement.area, placement.dock);
        // After re-adding: a floating dock stays floating when added to an area.
        placement.dock->setFloating(false);
        placement.dock->show();
    }

    defaultSizesPending_ = true;
    if (isVisible()) {
        applyDefaultPanelSizes();
    }
}

void MainWindow::applyDefaultPanelSizes() {
    // Proportions that work from 1366x768 up to 4K: side panels about a quarter and a fifth
    // of the width, the timeline about 40% of the height.
    const int w = width();
    const int h = height();
    resizeDocks({mediaBinDock_, inspectorDock_}, {w * 24 / 100, w * 20 / 100}, Qt::Horizontal);
    resizeDocks({timelineDock_}, {h * 40 / 100}, Qt::Vertical);
    defaultSizesPending_ = false;
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    if (defaultSizesPending_) {
        applyDefaultPanelSizes();
    }
}

void MainWindow::restoreLayout(const QSettings& settings) {
    const QByteArray geometry = settings.value(kGeometryKey).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    const QByteArray state = settings.value(kStateKey).toByteArray();
    if (!state.isEmpty() && restoreState(state, kLayoutVersion)) {
        defaultSizesPending_ = false; // the saved layout includes the panel sizes
    } else {
        resetLayout();
    }
}

void MainWindow::saveLayout(QSettings& settings) const {
    settings.setValue(kGeometryKey, saveGeometry());
    settings.setValue(kStateKey, saveState(kLayoutVersion));
}

void MainWindow::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    auto* quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    quitAction->setMenuRole(QAction::QuitRole);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(quitAction);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    for (QDockWidget* dock : {mediaBinDock_, inspectorDock_, timelineDock_}) {
        viewMenu->addAction(dock->toggleViewAction());
    }
    viewMenu->addSeparator();
    auto* resetAction = new QAction(tr("&Reset Layout"), this);
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetLayout);
    viewMenu->addAction(resetAction);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    auto* aboutAction = new QAction(tr("&About Cutline"), this);
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAction);
    auto* aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::showAbout() {
    QMessageBox::about(this, tr("About Cutline"),
                       tr("<h3>Cutline %1</h3>"
                          "<p>A lightweight video, audio and image editor.</p>"
                          "<p>Built with Qt %2.</p>")
                           .arg(QString::fromLatin1(kAppVersion), QString::fromLatin1(qVersion())));
}

} // namespace cutline::app
