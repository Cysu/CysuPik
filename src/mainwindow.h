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
    void displayPerspectivePanel();

    void processAntiColor();
    void processThreshold(int value);
    void processHistogramEqualization();
    void processHorizontalMirror();
    void processVerticalMirror();
    void processHorizontalScaling(int value);
    void processVerticalScaling(int value);
    void processRotation(int value);
    void processPerspectiveX(int value);
    void processPerspectiveY(int value);
    void processPerspectiveZ(int value);
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
    EDIT_TYPE prevEditType;

    QLabel* imageLabel;
    QScrollArea* mainPanel;
    FloatPanel* histogramPanel;
    FloatPanel* thresholdPanel;
    FloatPanel* scalingPanel;
    FloatPanel* rotationPanel;
    FloatPanel* perspectivePanel;

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
    QAction* perspectiveAct;
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
