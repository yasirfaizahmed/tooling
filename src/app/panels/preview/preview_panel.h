#pragma once

#include <QFrame>

class QLabel;

namespace cutline::app {

/// The program monitor: video viewport plus a transport bar with the current timecode and
/// the duration. Shows black until playback lands.
class PreviewPanel : public QFrame {
    Q_OBJECT

public:
    explicit PreviewPanel(QWidget* parent = nullptr);

    [[nodiscard]] QString positionText() const;

private:
    QLabel* position_ = nullptr;
    QLabel* duration_ = nullptr;
};

} // namespace cutline::app
