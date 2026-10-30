#include "panels/empty_state.h"

#include <QLabel>
#include <QVBoxLayout>

namespace cutline::app {

EmptyState::EmptyState(const QString& title, const QString& hint, QWidget* parent)
    : QWidget(parent) {
    auto* titleLabel = new QLabel(title, this);
    titleLabel->setObjectName(QStringLiteral("emptyStateTitle"));
    titleLabel->setAlignment(Qt::AlignCenter);

    auto* hintLabel = new QLabel(hint, this);
    hintLabel->setObjectName(QStringLiteral("emptyStateHint"));
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 16, 24, 16);
    layout->setSpacing(6);
    // No per-widget alignment flags: they break height-for-width of word-wrapped labels.
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addStretch();
}

} // namespace cutline::app
