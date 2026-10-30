#include "panels/inspector/inspector_panel.h"

#include "panels/empty_state.h"

#include <QVBoxLayout>

namespace cutline::app {

InspectorPanel::InspectorPanel(QWidget* parent) : QFrame(parent) {
    setObjectName(QStringLiteral("inspectorPanel"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new EmptyState(tr("Nothing selected"),
                                     tr("Select a clip to see and edit its properties."), this));
}

} // namespace cutline::app
