#include "chart.h"

Chart::Chart(const vector<int>& x, const vector<int>& y, QWidget *parent) : QWidget(parent) {
    this->x = x;
    this->y = y;
    maxX = maxY = 0;
    minY = minX = 2147483647;
    for (int i = 0; i < x.size(); i ++) {
        if (maxX < x[i]) maxX = x[i];
        if (minX > x[i]) minX = x[i];
    }
    for (int i = 0; i < y.size(); i ++) {
        if (maxY < y[i]) maxY = y[i];
        if (minY > y[i]) minY = y[i];
    }
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QSize Chart::sizeHint() const {
    return QSize(300, 100);
}

void Chart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.translate(0, height());
    painter.drawLine(0, 0, width(), 0);
    //painter.drawLine(0, 0, 0, -height());
}
