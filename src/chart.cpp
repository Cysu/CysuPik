#include "chart.h"
#include <cstdio>

Chart::Chart(QWidget *parent) : QWidget(parent) {
    this->resize(500, 100);
    update();
}

void Chart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(1);
    painter.drawLine(QLineF(10.0, 80.0, 90.0, 20.0));
}
