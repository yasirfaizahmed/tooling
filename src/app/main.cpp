#include "app_info.h"
#include "main_window.h"
#include "theme/theme.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[]) {
    // Use the exact display scale (e.g. 150%) instead of rounding it, for crisp HiDPI layouts.
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    QApplication::setApplicationName(QString::fromLatin1(cutline::app::kAppName));
    QApplication::setApplicationVersion(QString::fromLatin1(cutline::app::kAppVersion));
    QApplication::setOrganizationName(QString::fromLatin1(cutline::app::kAppName));
    QApplication::setOrganizationDomain(QString::fromLatin1(cutline::app::kOrganizationDomain));

    cutline::app::theme::apply(app, cutline::app::theme::dark());

    cutline::app::MainWindow window;
    window.restoreLayout(QSettings());
    QObject::connect(&app, &QApplication::aboutToQuit, &window, [&window] {
        QSettings settings;
        window.saveLayout(settings);
    });
    window.show();

    return QApplication::exec();
}
