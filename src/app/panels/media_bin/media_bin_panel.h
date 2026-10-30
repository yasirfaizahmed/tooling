#pragma once

#include <QFrame>

namespace cutline::app {

/// Imported media with thumbnails, duration and file info. Empty until import lands.
class MediaBinPanel : public QFrame {
    Q_OBJECT

public:
    explicit MediaBinPanel(QWidget* parent = nullptr);
};

} // namespace cutline::app
