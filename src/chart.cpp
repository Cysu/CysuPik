#include "chart.h"

Chart::Chart(int* y, QWidget *parent) : QWidget(parent) {
    this->y = y;
    maxY = 0;
    for (int i = 0; i < 256; i ++) {
        if (maxY < y[i]) maxY = y[i];
    }
    setFixedSize(257, 100);
}

void Chart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int w = width() - 1, h = height() - 1;
    painter.drawLine(0, h, 0, 0);
    painter.drawLine(0, h, w, h);
    for (int i = 0; i < 256; i ++) {
        painter.drawLine(i + 1, h, i + 1, (int)(h - y[i] * 100.0 / maxY));
    }
}
