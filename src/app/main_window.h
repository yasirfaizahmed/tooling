#pragma once

#include <QMainWindow>

class QDockWidget;
class QSettings;

namespace cutline::app {

class InspectorPanel;
class MediaBinPanel;
class PreviewPanel;
class TimelinePanel;

/// The editor window: Media Bin (left), Preview (centre), Inspector (right) and a
/// full-width Timeline (bottom). Every panel except the preview is a dock that can be
/// resized, moved, floated or closed; View > Reset Layout restores the default.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    [[nodiscard]] QDockWidget* mediaBinDock() const { return mediaBinDock_; }
    [[nodiscard]] QDockWidget* inspectorDock() const { return inspectorDock_; }
    [[nodiscard]] QDockWidget* timelineDock() const { return timelineDock_; }
    [[nodiscard]] PreviewPanel* previewPanel() const { return previewPanel_; }

    /// Restores the window geometry and panel layout written by saveLayout(). Keeps the default
    /// layout when nothing was saved or the saved layout is from an incompatible version.
    void restoreLayout(const QSettings& settings);
    void saveLayout(QSettings& settings) const;

public slots:
    /// Puts every panel back in its default place and size, reopening closed panels.
    void resetLayout();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void applyDefaultPanelSizes();
    QDockWidget* createDock(QWidget* panel, const QString& title, const QString& objectName);
    void createMenus();
    void showAbout();

    PreviewPanel* previewPanel_ = nullptr;
    MediaBinPanel* mediaBinPanel_ = nullptr;
    InspectorPanel* inspectorPanel_ = nullptr;
    TimelinePanel* timelinePanel_ = nullptr;

    QDockWidget* mediaBinDock_ = nullptr;
    QDockWidget* inspectorDock_ = nullptr;
    QDockWidget* timelineDock_ = nullptr;

    /// Default sizes are proportions of the window, so they wait until it has its final size.
    bool defaultSizesPending_ = true;
};

} // namespace cutline::app
