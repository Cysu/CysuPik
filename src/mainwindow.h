#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <vector>
#include "floatpanel.h"
#include "chart.h"
#include "imageeditor.h"
#include "utils.h"
#include "const.h"

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
    void displayNeighborAvePanel();
    void displayNeighborMedPanel();

    void processAntiColor();
    void processThreshold(int value);
    void processHistogramEqualization();
    void processHorizontalMirror();
    void processVerticalMirror();
    void processScaling(int value);
    void processRotation(int value);
    void processPerspective(int value);
    void processErosion();
    void processDilation();
    void processOpenOpr();
    void processCloseOpr();
    void processThinning();
    void processNeighborAve(int value);
    void processNeighborMed(int value);
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
    QImage *originImage, *previewImage;
    ACTION_TYPE lastActType;

    QLabel* imageLabel;
    QScrollArea* mainPanel;
    FloatPanel* histogramPanel;
    FloatPanel* thresholdPanel;
    FloatPanel* scalingPanel;
    FloatPanel* rotationPanel;
    FloatPanel* perspectivePanel;
    FloatPanel* neighborAvePanel;
    FloatPanel* neighborMedPanel;

    QSlider* thresholdSlider;
    QSlider* rotationSlider;
    QSlider* scalingHSlider;
    QSlider* scalingVSlider;
    QSlider* perspectiveXSlider;
    QSlider* perspectiveYSlider;
    QSlider* perspectiveZSlider;
    QSlider* neighborAveSlider;
    QSlider* neighborMedSlider;

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
    QAction* erosionAct;
    QAction* dilationAct;
    QAction* openOprAct;
    QAction* closeOprAct;
    QAction* thinningAct;
    QAction* neighborAveAct;
    QAction* neighborMedAct;
    QAction* hazeAct;

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* processMenu;
    QMenu* pointOperationMenu;
    QMenu* geoOperationMenu;
    QMenu* morphOperationMenu;
    QMenu* neighborOperationMenu;
    QMenu* otherOperationMenu;

    void clearStack();
    void addPreview();
    void recordAct(ACTION_TYPE type);
    void afterAct(ACTION_TYPE type);
    bool isSlideAction(ACTION_TYPE type);

};

#endif // MAINWINDOW_H
