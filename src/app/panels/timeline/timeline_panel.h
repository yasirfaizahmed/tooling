#pragma once

#include <QFrame>

namespace cutline::app {

/// The multi-track timeline. Empty until the timeline widget lands.
class TimelinePanel : public QFrame {
    Q_OBJECT

public:
    explicit TimelinePanel(QWidget* parent = nullptr);
};

} // namespace cutline::app
