#pragma once

#include <QFrame>

class QHBoxLayout;
class QLabel;

namespace cutline::app {

/// The compact title strip on top of every panel, so docked panels and the central preview
/// look alike. Used as a QDockWidget title bar it passes mouse presses through, so the dock
/// can still be dragged and double-clicked to float.
class PanelTitleBar : public QFrame {
    Q_OBJECT

public:
    explicit PanelTitleBar(const QString& title, QWidget* parent = nullptr);

    [[nodiscard]] QString title() const;

    /// Right-aligned area for panel-specific buttons.
    [[nodiscard]] QHBoxLayout* actionsLayout() const { return actions_; }

private:
    QLabel* titleLabel_ = nullptr;
    QHBoxLayout* actions_ = nullptr;
};

} // namespace cutline::app
