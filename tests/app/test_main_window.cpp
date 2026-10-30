#include "main_window.h"
#include "panels/preview/preview_panel.h"
#include "theme/theme.h"

#include <QApplication>
#include <QDockWidget>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

using cutline::app::MainWindow;
namespace theme = cutline::app::theme;

class MainWindowTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void showsStandardPanelLayout();
    void resetLayoutReopensClosedPanels();
    void layoutRoundTripsThroughSettings();
    void ignoresMissingSavedLayout();
    void themeFillsEveryStyleToken();
};

namespace {

void showAndWait(MainWindow& window) {
    window.resize(1600, 900);
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
}

} // namespace

void MainWindowTest::initTestCase() {
    theme::apply(*qApp, theme::dark());
}

void MainWindowTest::showsStandardPanelLayout() {
    MainWindow window;
    showAndWait(window);

    QCOMPARE(window.centralWidget(), window.previewPanel());
    QCOMPARE(window.dockWidgetArea(window.mediaBinDock()), Qt::LeftDockWidgetArea);
    QCOMPARE(window.dockWidgetArea(window.inspectorDock()), Qt::RightDockWidgetArea);
    QCOMPARE(window.dockWidgetArea(window.timelineDock()), Qt::BottomDockWidgetArea);

    QWidget* const panels[] = {window.mediaBinDock(), window.previewPanel(), window.inspectorDock(),
                               window.timelineDock()};
    for (QWidget* panel : panels) {
        QVERIFY2(panel->isVisible(), qPrintable(panel->objectName()));
        QVERIFY2(!panel->geometry().isEmpty(), qPrintable(panel->objectName()));
    }

    // Media Bin | Preview | Inspector across the top, the Timeline below at full width.
    const QRect bin = window.mediaBinDock()->geometry();
    const QRect preview = window.previewPanel()->geometry();
    const QRect inspector = window.inspectorDock()->geometry();
    const QRect timeline = window.timelineDock()->geometry();
    QVERIFY(bin.right() < preview.left());
    QVERIFY(preview.right() < inspector.left());
    QVERIFY(timeline.top() > preview.bottom());
    QVERIFY(timeline.width() > window.width() * 9 / 10);
    QCOMPARE(window.previewPanel()->positionText(), QStringLiteral("00:00:00:00"));

    // Set CUTLINE_SCREENSHOT=<file.png> to capture the window, e.g. for docs or CI artifacts.
    const QString screenshotPath = qEnvironmentVariable("CUTLINE_SCREENSHOT");
    if (!screenshotPath.isEmpty()) {
        QVERIFY(window.grab().save(screenshotPath));
    }
}

void MainWindowTest::resetLayoutReopensClosedPanels() {
    MainWindow window;
    showAndWait(window);

    window.inspectorDock()->close();
    window.timelineDock()->setFloating(true);
    QVERIFY(!window.inspectorDock()->isVisible());

    window.resetLayout();
    QVERIFY(window.inspectorDock()->isVisible());
    QVERIFY(!window.timelineDock()->isFloating());
    QCOMPARE(window.dockWidgetArea(window.inspectorDock()), Qt::RightDockWidgetArea);
    QCOMPARE(window.dockWidgetArea(window.timelineDock()), Qt::BottomDockWidgetArea);
}

void MainWindowTest::layoutRoundTripsThroughSettings() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QSettings settings(dir.filePath(QStringLiteral("layout.ini")), QSettings::IniFormat);

    {
        MainWindow window;
        showAndWait(window);
        window.mediaBinDock()->close();
        window.saveLayout(settings);
    }

    MainWindow restored;
    restored.restoreLayout(settings);
    showAndWait(restored);
    QVERIFY(restored.mediaBinDock()->isHidden());
    QVERIFY(restored.inspectorDock()->isVisible());
    QVERIFY(restored.timelineDock()->isVisible());
}

void MainWindowTest::ignoresMissingSavedLayout() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QSettings empty(dir.filePath(QStringLiteral("empty.ini")), QSettings::IniFormat);

    MainWindow window;
    window.restoreLayout(empty);
    showAndWait(window);
    QVERIFY(window.mediaBinDock()->isVisible());
    QVERIFY(window.inspectorDock()->isVisible());
    QVERIFY(window.timelineDock()->isVisible());
}

void MainWindowTest::themeFillsEveryStyleToken() {
    const QString sheet = theme::makeStyleSheet(theme::dark());
    QVERIFY(!sheet.isEmpty());
    QVERIFY2(!sheet.contains(QLatin1Char('@')), "unreplaced @token@ in style.qss");
    QCOMPARE(qApp->palette().color(QPalette::Window), theme::dark().panel);
}

QTEST_MAIN(MainWindowTest)
#include "test_main_window.moc"
