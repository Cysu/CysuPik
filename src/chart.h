#ifndef CHART_H
#define CHART_H

#include <QtGui>
#include <vector>

using namespace std;

class Chart : public QWidget
{
    Q_OBJECT
public:
    explicit Chart(const vector<int>& x, const vector<int>& y, QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    vector<int> x, y;
    int maxY, maxX;
    int minY, minX;
};

#endif // CHART_H
