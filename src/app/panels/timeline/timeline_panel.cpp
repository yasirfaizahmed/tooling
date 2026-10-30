#include "panels/timeline/timeline_panel.h"

#include "panels/empty_state.h"

#include <QVBoxLayout>

namespace cutline::app {

TimelinePanel::TimelinePanel(QWidget* parent) : QFrame(parent) {
    setObjectName(QStringLiteral("timelinePanel"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new EmptyState(tr("Empty timeline"),
                                     tr("Video and audio tracks with your clips will appear here."),
                                     this));
}

} // namespace cutline::app
