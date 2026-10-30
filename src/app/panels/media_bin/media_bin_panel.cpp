#include "panels/media_bin/media_bin_panel.h"

#include "panels/empty_state.h"

#include <QVBoxLayout>

namespace cutline::app {

MediaBinPanel::MediaBinPanel(QWidget* parent) : QFrame(parent) {
    setObjectName(QStringLiteral("mediaBinPanel"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new EmptyState(
        tr("No media yet"), tr("Imported video, audio and image files will appear here."), this));
}

} // namespace cutline::app
