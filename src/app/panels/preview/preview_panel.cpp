#include "panels/preview/preview_panel.h"

#include "cutline/core/timecode.h"
#include "panels/panel_title_bar.h"

#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace cutline::app {

namespace {

QString timecodeText(core::FrameIndex frame) {
    return QString::fromStdString(core::formatTimecode(frame, core::frame_rates::k30));
}

} // namespace

PreviewPanel::PreviewPanel(QWidget* parent) : QFrame(parent) {
    setObjectName(QStringLiteral("previewPanel"));

    auto* viewport = new QFrame(this);
    viewport->setObjectName(QStringLiteral("previewViewport"));
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* viewportHint = new QLabel(tr("No media loaded"), viewport);
    viewportHint->setObjectName(QStringLiteral("previewViewportHint"));
    auto* viewportLayout = new QVBoxLayout(viewport);
    viewportLayout->addWidget(viewportHint, 0, Qt::AlignCenter);

    const QFont monospace = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    position_ = new QLabel(timecodeText(0), this);
    position_->setObjectName(QStringLiteral("previewPosition"));
    position_->setFont(monospace);
    duration_ = new QLabel(timecodeText(0), this);
    duration_->setObjectName(QStringLiteral("previewDuration"));
    duration_->setFont(monospace);

    auto* transport = new QHBoxLayout;
    transport->setContentsMargins(10, 6, 10, 6);
    transport->addWidget(position_);
    transport->addStretch();
    transport->addWidget(duration_);

    auto* body = new QVBoxLayout;
    body->setContentsMargins(8, 8, 8, 0);
    body->addWidget(viewport, 1);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(new PanelTitleBar(tr("Preview"), this));
    layout->addLayout(body, 1);
    layout->addLayout(transport);
}

QString PreviewPanel::positionText() const {
    return position_->text();
}

} // namespace cutline::app
