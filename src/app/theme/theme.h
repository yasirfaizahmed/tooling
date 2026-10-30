#pragma once

#include <QColor>
#include <QPalette>
#include <QString>

class QApplication;

namespace cutline::app::theme {

/// Colour tokens shared by the widget palette, the style sheet and custom-painted widgets
/// such as the timeline. A light theme is just another set of these tokens.
struct Colors {
    QColor window; ///< Behind the panels: gaps and splitters.
    QColor panel;  ///< Panel bodies.
    QColor raised; ///< Title bars, menus, buttons.
    QColor sunken; ///< Text fields, lists, pressed buttons.
    QColor border;
    QColor text;
    QColor textMuted;
    QColor accent;     ///< Selection, focus, drop targets.
    QColor accentText; ///< Text drawn on the accent colour.
};

[[nodiscard]] const Colors& dark();

[[nodiscard]] QPalette makePalette(const Colors& colors);

/// The application style sheet with every @token@ placeholder replaced by its colour.
[[nodiscard]] QString makeStyleSheet(const Colors& colors);

/// Applies the Fusion style plus the palette and style sheet for `colors` app-wide.
void apply(QApplication& app, const Colors& colors);

} // namespace cutline::app::theme
