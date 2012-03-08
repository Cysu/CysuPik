#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <vector>
#include "floatpanel.h"
#include "chart.h"
#include "imageeditor.h"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void open();
    void processHistogramEqualization();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void createPanels();
    void createActions();
    void createMenus();

    ImageEditor imageEditor;
    vector<QImage> images;

    QLabel* imageLabel;
    QScrollArea* mainPanel;

    QAction* openAct;
    QAction* histogramEqualizationAct;

    QMenu* fileMenu;
    QMenu* processMenu;
    QMenu* pointOperationMenu;

};

#endif // MAINWINDOW_H
