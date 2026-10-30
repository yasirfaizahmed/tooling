#pragma once

#include <QFrame>

namespace cutline::app {

/// Properties of the selected clip (transform, audio, color...). Empty until selection lands.
class InspectorPanel : public QFrame {
    Q_OBJECT

public:
    explicit InspectorPanel(QWidget* parent = nullptr);
};

} // namespace cutline::app
