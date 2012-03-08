#ifndef FLOATPANEL_H
#define FLOATPANEL_H

#include <QtGui>

class FloatPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FloatPanel(const QString& title, QWidget* widget);

signals:

public slots:

private:
};

#endif // FLOATPANEL_H
