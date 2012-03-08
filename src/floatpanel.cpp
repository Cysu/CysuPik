#include "floatpanel.h"

FloatPanel::FloatPanel(const QString& title, QWidget* widget) {
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(widget);

    QGroupBox* frame = new QGroupBox(title);
    frame->setLayout(layout);

    setWidget(frame);
}
