#pragma once

#include <QWidget>

namespace cutline::app {

/// Centred title and hint shown by a panel that has nothing to display yet.
class EmptyState : public QWidget {
    Q_OBJECT

public:
    EmptyState(const QString& title, const QString& hint, QWidget* parent = nullptr);
};

} // namespace cutline::app
