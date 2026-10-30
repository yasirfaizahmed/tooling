#include "panels/panel_title_bar.h"

#include <QHBoxLayout>
#include <QLabel>

namespace cutline::app {

PanelTitleBar::PanelTitleBar(const QString& title, QWidget* parent) : QFrame(parent) {
    setObjectName(QStringLiteral("panelTitleBar"));

    titleLabel_ = new QLabel(title, this);
    titleLabel_->setObjectName(QStringLiteral("panelTitle"));
    QFont font = titleLabel_->font();
    font.setCapitalization(QFont::AllUppercase);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    font.setWeight(QFont::DemiBold);
    font.setPointSizeF(font.pointSizeF() * 0.85);
    titleLabel_->setFont(font);

    actions_ = new QHBoxLayout;
    actions_->setContentsMargins(0, 0, 0, 0);
    actions_->setSpacing(2);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 6, 5);
    layout->addWidget(titleLabel_);
    layout->addStretch();
    layout->addLayout(actions_);
}

QString PanelTitleBar::title() const {
    return titleLabel_->text();
}

} // namespace cutline::app
