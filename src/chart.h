#ifndef CHART_H
#define CHART_H

#include <QtGui>
#include <vector>

using namespace std;

class Chart : public QWidget
{
    Q_OBJECT
public:
    explicit Chart(int* y, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int* y;
    int maxY;
};

#endif // CHART_H
