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
    void displayHistogramPanel(QImage* image = NULL);
    void displayThresholdPanel();
    void displayScalingPanel();
    void displayRotationPanel();
    void processAntiColor();
    void processThreshold(int value);
    void processHistogramEqualization();
    void processHorizontalMirror();
    void processVerticalMirror();
    void processHorizontalScaling(int value);
    void processVerticalScaling(int value);
    void processRotation(int value);
    void processHaze();

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
    QImage* previewImage;

    QLabel* imageLabel;
    QScrollArea* mainPanel;
    FloatPanel* histogramPanel;
    FloatPanel* thresholdPanel;
    FloatPanel* horizontalScalingPanel;
    FloatPanel* verticalScalingPanel;
    FloatPanel* rotationPanel;

    QAction* openAct;
    QAction* undoAct;
    QAction* redoAct;
    QAction* antiColorAct;
    QAction* thresholdAct;
    QAction* histogramEqualizationAct;
    QAction* horizontalMirrorAct;
    QAction* verticalMirrorAct;
    QAction* scalingAct;
    QAction* rotationAct;
    QAction* hazeAct;

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* processMenu;
    QMenu* pointOperationMenu;
    QMenu* geoOperationMenu;
    QMenu* otherOperationMenu;

    void clearStack();
    void addPreview();

};

#endif // MAINWINDOW_H
