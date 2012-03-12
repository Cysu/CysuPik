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
    void undo();
    void redo();
    void displayHistogramPanel();
    void displayThresholdPanel();
    void processAntiColor();
    void processThresholdMove(int value);
    void processThresholdBegin();
    void processThresholdEnd();
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
    int cntImageNum;

    QLabel* imageLabel;
    QScrollArea* mainPanel;
    FloatPanel* histogramPanel;
    FloatPanel* thresholdPanel;

    QAction* openAct;
    QAction* undoAct;
    QAction* redoAct;
    QAction* antiColorAct;
    QAction* thresholdAct;
    QAction* histogramEqualizationAct;

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* processMenu;
    QMenu* pointOperationMenu;

    void clearStack();

};

#endif // MAINWINDOW_H
