#include "theme/theme.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>

#include <utility>

namespace cutline::app::theme {

const Colors& dark() {
    static const Colors colors{
        .window = QColor(0x16, 0x18, 0x1c),
        .panel = QColor(0x1e, 0x21, 0x26),
        .raised = QColor(0x27, 0x2b, 0x31),
        .sunken = QColor(0x12, 0x14, 0x17),
        .border = QColor(0x31, 0x36, 0x3e),
        .text = QColor(0xe3, 0xe6, 0xea),
        .textMuted = QColor(0x8a, 0x91, 0x9c),
        .accent = QColor(0x3d, 0x8b, 0xfd),
        .accentText = QColor(0xff, 0xff, 0xff),
    };
    return colors;
}

QPalette makePalette(const Colors& colors) {
    QPalette palette;
    palette.setColor(QPalette::Window, colors.panel);
    palette.setColor(QPalette::WindowText, colors.text);
    palette.setColor(QPalette::Base, colors.sunken);
    palette.setColor(QPalette::AlternateBase, colors.panel);
    palette.setColor(QPalette::ToolTipBase, colors.raised);
    palette.setColor(QPalette::ToolTipText, colors.text);
    palette.setColor(QPalette::PlaceholderText, colors.textMuted);
    palette.setColor(QPalette::Text, colors.text);
    palette.setColor(QPalette::Button, colors.raised);
    palette.setColor(QPalette::ButtonText, colors.text);
    palette.setColor(QPalette::BrightText, colors.accentText);
    palette.setColor(QPalette::Highlight, colors.accent);
    palette.setColor(QPalette::HighlightedText, colors.accentText);
    palette.setColor(QPalette::Link, colors.accent);
    palette.setColor(QPalette::Light, colors.raised.lighter(125));
    palette.setColor(QPalette::Midlight, colors.raised);
    palette.setColor(QPalette::Mid, colors.border);
    palette.setColor(QPalette::Dark, colors.sunken);
    palette.setColor(QPalette::Shadow, Qt::black);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    palette.setColor(QPalette::Accent, colors.accent);
#endif

    for (const QPalette::ColorRole role :
         {QPalette::WindowText, QPalette::Text, QPalette::ButtonText}) {
        palette.setColor(QPalette::Disabled, role, colors.textMuted);
    }
    palette.setColor(QPalette::Disabled, QPalette::Highlight, colors.border);
    return palette;
}

QString makeStyleSheet(const Colors& colors) {
    QFile file(QStringLiteral(":/cutline/theme/style.qss"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Cutline: style sheet resource is missing");
        return {};
    }
    QString sheet = QString::fromUtf8(file.readAll());

    const std::pair<const char*, const QColor&> tokens[] = {
        {"@window@", colors.window},         {"@panel@", colors.panel},
        {"@raised@", colors.raised},         {"@sunken@", colors.sunken},
        {"@border@", colors.border},         {"@text@", colors.text},
        {"@textMuted@", colors.textMuted},   {"@accent@", colors.accent},
        {"@accentText@", colors.accentText},
    };
    for (const auto& [token, color] : tokens) {
        sheet.replace(QLatin1String(token), color.name());
    }
    return sheet;
}

void apply(QApplication& app, const Colors& colors) {
    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QApplication::setPalette(makePalette(colors));
    app.setStyleSheet(makeStyleSheet(colors));
}

} // namespace cutline::app::theme
