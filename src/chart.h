#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QtGui>

class Chart : public QWidget {
    Q_OBJECT
public:
    explicit Chart(QWidget *parent = 0);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent* event);

};

#endif // CHART_H
