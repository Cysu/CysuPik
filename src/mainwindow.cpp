#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pic"));
    showMaximized();

    cntImageNum = -1;
    originImage = NULL;
    previewImage = NULL;
    lastActType = NOTHING;
}

/* *
 * GUI methods.
 * */

void MainWindow::createPanels() {
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->resize(0, 0);

    mainPanel = new QScrollArea;
    mainPanel->setBackgroundRole(QPalette::Dark);
    mainPanel->setWidget(imageLabel);
    mainPanel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setCentralWidget(mainPanel);

    histogramPanel = NULL;
    thresholdPanel = NULL;
    scalingPanel = NULL;
    rotationPanel = NULL;
    perspectivePanel = NULL;
    neighborAvePanel = NULL;
    neighborMedPanel = NULL;
    neighborGaussianPanel = NULL;

    thresholdSlider = NULL;
    rotationSlider = NULL;
    scalingHSlider = NULL;
    scalingVSlider = NULL;
    perspectiveXSlider = NULL;
    perspectiveYSlider = NULL;
    perspectiveZSlider = NULL;
    neighborAveSlider = NULL;
    neighborMedSlider = NULL;
    neighborGaussianRSlider = NULL;
    neighborGaussianSSlider = NULL;
}

void MainWindow::createActions() {
    CREATE_ACTION(openAct, "&Open", "Ctrl+O", open);
    CREATE_ACTION(undoAct, "&Undo", "Ctrl+Z", undo);
    CREATE_ACTION(redoAct, "&Redo", "Ctrl+Y", redo);
    CREATE_ACTION(showHistogramAct, "Show Histogram", "", displayHistogramPanel);
    CREATE_ACTION(convertToGrayscaleAct, "Convert To Grayscale", "", processConvertToGrayscale);
    CREATE_ACTION(antiColorAct, "Anti Color", "", processAntiColor);
    CREATE_ACTION(thresholdAct, "Threshold", "Ctrl+H", displayThresholdPanel);
    CREATE_ACTION(histogramEqualizationAct, "Histogram Equalization", "Ctrl+B", processHistogramEqualization);
    CREATE_ACTION(horizontalMirrorAct, "Horizontal Mirror", "", processHorizontalMirror);
    CREATE_ACTION(verticalMirrorAct, "Vertical Mirror", "", processVerticalMirror);
    CREATE_ACTION(scalingAct, "Scaling", "", displayScalingPanel);
    CREATE_ACTION(rotationAct, "Rotation", "", displayRotationPanel);
    CREATE_ACTION(perspectiveAct, "Perspective", "", displayPerspectivePanel);
    CREATE_ACTION(erosionAct, "Erosion", "Ctrl+E", processErosion);
    CREATE_ACTION(dilationAct, "Dilation", "Ctrl+D", processDilation);
    CREATE_ACTION(openOprAct, "Open Operation", "", processOpenOpr);
    CREATE_ACTION(closeOprAct, "Close Operation", "", processCloseOpr);
    CREATE_ACTION(thinningAct, "Thinning", "Ctrl+T", processThinning);
    CREATE_ACTION(neighborAveAct, "Neighborhood Averaging", "", displayNeighborAvePanel);
    CREATE_ACTION(neighborMedAct, "Neighborhood Median", "", displayNeighborMedPanel);
    CREATE_ACTION(neighborGaussianAct, "Neighborhood Gaussian", "Ctrl+G", displayNeighborGaussianPanel);
    CREATE_ACTION(sobelAct, "Sobel", "Ctrl+S", processSobel);
    CREATE_ACTION(robertsAct, "Roberts", "Ctrl+R", processRoberts);
    CREATE_ACTION(cannyAct, "Canny", "Ctrl+C", processCanny);
    CREATE_ACTION(hazeAct, "Haze", "", processHaze);
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    processMenu = new QMenu(tr("&Process"), this);
    pointOperationMenu = processMenu->addMenu(tr("&Point Operation"));
    pointOperationMenu->addAction(showHistogramAct);
    pointOperationMenu->addAction(convertToGrayscaleAct);
    pointOperationMenu->addAction(antiColorAct);
    pointOperationMenu->addAction(thresholdAct);
    pointOperationMenu->addAction(histogramEqualizationAct);
    geoOperationMenu = processMenu->addMenu(tr("&Geometrical Operation"));
    geoOperationMenu->addAction(horizontalMirrorAct);
    geoOperationMenu->addAction(verticalMirrorAct);
    geoOperationMenu->addAction(scalingAct);
    geoOperationMenu->addAction(rotationAct);
    geoOperationMenu->addAction(perspectiveAct);
    morphOperationMenu = processMenu->addMenu(tr("&Morphological Operation"));
    morphOperationMenu->addAction(erosionAct);
    morphOperationMenu->addAction(dilationAct);
    morphOperationMenu->addAction(openOprAct);
    morphOperationMenu->addAction(closeOprAct);
    morphOperationMenu->addAction(thinningAct);
    neighborOperationMenu = processMenu->addMenu(tr("&Neighborhood Averaging"));
    neighborOperationMenu->addAction(neighborAveAct);
    neighborOperationMenu->addAction(neighborMedAct);
    neighborOperationMenu->addAction(neighborGaussianAct);
    neighborOperationMenu->addAction(sobelAct);
    neighborOperationMenu->addAction(robertsAct);
    neighborOperationMenu->addAction(cannyAct);
    otherOperationMenu = processMenu->addMenu(tr("&Other Operation"));
    otherOperationMenu->addAction(hazeAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(processMenu);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (originImage != NULL) {
            imageLabel->setPixmap(QPixmap::fromImage(*originImage));
            imageLabel->adjustSize();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (previewImage != NULL) {
            imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
            imageLabel->adjustSize();
        }
    }
}

/* *
 * Action response.
 * */

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage* image = new QImage(fileName);
        if (image->isNull()) {
            QMessageBox::information(this, tr("Cysu Pic"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        recordAct(OTHER_OPEN);
        delete previewImage;
        originImage = previewImage = image;
        afterAct(OTHER_OPEN);
    }
}

void MainWindow::undo() {
    addPreview();
    if (cntImageNum > 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[-- cntImageNum]));
        imageLabel->adjustSize();
        lastActType = NOTHING;
        originImage = NULL;
        previewImage = NULL;
    }
}

void MainWindow::redo() {
    if (cntImageNum < images.size() - 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[++ cntImageNum]));
        imageLabel->adjustSize();
        lastActType = NOTHING;
        originImage = NULL;
        previewImage = NULL;
    }
}

void MainWindow::displayHistogramPanel(QImage* image) {
    int* tmpH = new int[256];
    if (image == NULL)
        ImageEditor::getHistogram(previewImage, tmpH);
    else
        ImageEditor::getHistogram(image, tmpH);
    delete histogramPanel;
    histogramPanel = new FloatPanel(tr("Histogram"), new Chart(tmpH));
    addDockWidget(Qt::RightDockWidgetArea, histogramPanel);
}

void MainWindow::displayThresholdPanel() {
    delete thresholdSlider;
    thresholdSlider = new QSlider(Qt::Horizontal, this);
    thresholdSlider->setMinimum(0);
    thresholdSlider->setMaximum(255);
    thresholdSlider->setFixedSize(256, 20);
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(processThreshold(int)));
    delete thresholdPanel;
    thresholdPanel = new FloatPanel(tr("Threshold"), thresholdSlider);
    addDockWidget(Qt::RightDockWidgetArea, thresholdPanel);
}

void MainWindow::displayScalingPanel() {
    delete scalingHSlider;
    scalingHSlider = new QSlider(Qt::Horizontal, this);
    scalingHSlider->setMinimum(1);
    scalingHSlider->setMaximum(200);
    scalingHSlider->setFixedSize(200, 20);
    scalingHSlider->setValue(100);
    connect(scalingHSlider, SIGNAL(valueChanged(int)), this, SLOT(processScaling(int)));

    delete scalingVSlider;
    scalingVSlider = new QSlider(Qt::Horizontal, this);
    scalingVSlider->setMinimum(1);
    scalingVSlider->setMaximum(200);
    scalingVSlider->setFixedSize(200, 20);
    scalingVSlider->setValue(100);
    connect(scalingVSlider, SIGNAL(valueChanged(int)), this, SLOT(processScaling(int)));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    QLabel* horizontalScalingLabel = new QLabel(tr("Horizontal"));
    QLabel* verticalScalingLabel = new QLabel(tr("Vertical"));
    layout->addWidget(horizontalScalingLabel, 0, 0);
    layout->addWidget(scalingHSlider, 0, 1);
    layout->addWidget(verticalScalingLabel, 1, 0);
    layout->addWidget(scalingVSlider, 1, 1);
    widget->setLayout(layout);

    delete scalingPanel;
    scalingPanel = new FloatPanel(tr("Scaling"), widget);
    addDockWidget(Qt::RightDockWidgetArea, scalingPanel);
}

void MainWindow::displayRotationPanel() {
    delete rotationSlider;
    rotationSlider = new QSlider(Qt::Horizontal, this);
    rotationSlider->setMinimum(0);
    rotationSlider->setMaximum(360);
    rotationSlider->setFixedSize(361, 20);
    connect(rotationSlider, SIGNAL(valueChanged(int)), this, SLOT(processRotation(int)));
    delete rotationPanel;
    rotationPanel = new FloatPanel(tr("Rotation"), rotationSlider);
    addDockWidget(Qt::RightDockWidgetArea, rotationPanel);
}

void MainWindow::displayPerspectivePanel() {
    perspectiveXSlider = new QSlider(Qt::Horizontal, this);
    perspectiveXSlider->setMinimum(-90);
    perspectiveXSlider->setMaximum(90);
    perspectiveXSlider->setFixedSize(200, 20);
    perspectiveXSlider->setValue(0);
    connect(perspectiveXSlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspective(int)));

    perspectiveYSlider = new QSlider(Qt::Horizontal, this);
    perspectiveYSlider->setMinimum(-90);
    perspectiveYSlider->setMaximum(90);
    perspectiveYSlider->setFixedSize(200, 20);
    perspectiveYSlider->setValue(0);
    connect(perspectiveYSlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspective(int)));

    perspectiveZSlider = new QSlider(Qt::Horizontal, this);
    perspectiveZSlider->setMinimum(50);
    perspectiveZSlider->setMaximum(150);
    perspectiveZSlider->setFixedSize(200, 20);
    perspectiveZSlider->setValue(100);
    connect(perspectiveZSlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspective(int)));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    QLabel* xLabel = new QLabel(tr("X"));
    QLabel* yLabel = new QLabel(tr("Y"));
    QLabel* zLabel = new QLabel(tr("Z"));
    layout->addWidget(xLabel, 0, 0);
    layout->addWidget(perspectiveXSlider, 0, 1);
    layout->addWidget(yLabel, 1, 0);
    layout->addWidget(perspectiveYSlider, 1, 1);
    layout->addWidget(zLabel, 2, 0);
    layout->addWidget(perspectiveZSlider, 2, 1);
    widget->setLayout(layout);

    delete perspectivePanel;
    perspectivePanel = new FloatPanel(tr("Perspective"), widget);
    addDockWidget(Qt::RightDockWidgetArea, perspectivePanel);
}

void MainWindow::displayNeighborAvePanel() {
    delete neighborAveSlider;
    neighborAveSlider = new QSlider(Qt::Horizontal, this);
    neighborAveSlider->setMinimum(0);
    neighborAveSlider->setMaximum(5);
    neighborAveSlider->setFixedSize(140, 20);
    connect(neighborAveSlider, SIGNAL(valueChanged(int)), this, SLOT(processNeighborAve(int)));
    delete neighborAvePanel;
    neighborAvePanel = new FloatPanel(tr("Neighborhood Averaging"), neighborAveSlider);
    addDockWidget(Qt::RightDockWidgetArea, neighborAvePanel);
}

void MainWindow::displayNeighborMedPanel() {
    delete neighborMedSlider;
    neighborMedSlider = new QSlider(Qt::Horizontal, this);
    neighborMedSlider->setMinimum(0);
    neighborMedSlider->setMaximum(5);
    neighborMedSlider->setFixedSize(140, 20);
    connect(neighborMedSlider, SIGNAL(valueChanged(int)), this, SLOT(processNeighborMed(int)));
    delete neighborMedPanel;
    neighborMedPanel = new FloatPanel(tr("Neighborhood Median"), neighborMedSlider);
    addDockWidget(Qt::RightDockWidgetArea, neighborMedPanel);
}

void MainWindow::displayNeighborGaussianPanel() {
    delete neighborGaussianRSlider;
    neighborGaussianRSlider = new QSlider(Qt::Horizontal, this);
    neighborGaussianRSlider->setMinimum(0);
    neighborGaussianRSlider->setMaximum(5);
    neighborGaussianRSlider->setFixedSize(140, 20);
    connect(neighborGaussianRSlider, SIGNAL(valueChanged(int)), this, SLOT(processNeighborGaussian(int)));

    delete neighborGaussianSSlider;
    neighborGaussianSSlider = new QSlider(Qt::Horizontal, this);
    neighborGaussianSSlider->setMinimum(1);
    neighborGaussianSSlider->setMaximum(500);
    neighborGaussianSSlider->setFixedSize(140, 20);
    connect(neighborGaussianSSlider, SIGNAL(valueChanged(int)), this, SLOT(processNeighborGaussian(int)));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    QLabel* gaussianRLabel = new QLabel(tr("R"));
    QLabel* gaussianSLabel = new QLabel(tr("S"));
    layout->addWidget(gaussianRLabel, 0, 0);
    layout->addWidget(neighborGaussianRSlider, 0, 1);
    layout->addWidget(gaussianSLabel, 1, 0);
    layout->addWidget(neighborGaussianSSlider, 1, 1);
    widget->setLayout(layout);

    delete neighborGaussianPanel;
    neighborGaussianPanel = new FloatPanel(tr("Neighborhood Gaussian"), widget);
    addDockWidget(Qt::RightDockWidgetArea, neighborGaussianPanel);
}

void MainWindow::processConvertToGrayscale() {
    DO_ACTION(POINT_CONVERTTOGRAYSCALE, convertToGrayscale());
}

void MainWindow::processAntiColor() {
    DO_ACTION(POINT_ANTICOLOR, antiColor());
}

void MainWindow::processThreshold(int value) {
    DO_ACTION(POINT_THRESHOLD, threshold(value));
}

void MainWindow::processHistogramEqualization() {
    DO_ACTION(POINT_HISTOGRAMNORMALIZATION, histogramEqualization());
}

void MainWindow::processHorizontalMirror() {
    DO_ACTION(GEOMETRICAL_HORIZONTALMIRROR, horizontalMirror());
}

void MainWindow::processVerticalMirror() {
    DO_ACTION(GEOMETRICAL_VERTICALMIRROR, verticalMirror());
}

void MainWindow::processScaling(int value) {
    int hValue = scalingHSlider->value(), vValue = scalingVSlider->value();
    DO_ACTION(GEOMETRICAL_SCALING, scaling(hValue, vValue));
}

void MainWindow::processRotation(int value) {
    DO_ACTION(GEOMETRICAL_ROTATION, rotation(value));
}

void MainWindow::processPerspective(int value) {
    int xValue = perspectiveXSlider->value(), yValue = perspectiveYSlider->value(), zValue = perspectiveZSlider->value();
    DO_ACTION(GEOMETRICAL_PERSPECTIVE, perspective(xValue, yValue, zValue));
}

void MainWindow::processErosion() {
    DO_ACTION(MORPHOLOGICAL_EROSION, erosion());
}

void MainWindow::processDilation() {
    DO_ACTION(MORPHOLOGICAL_DILATION, dilation());
}

void MainWindow::processOpenOpr() {
    DO_ACTION(MORPHOLOGICAL_OPEN, openOpr());
}

void MainWindow::processCloseOpr() {
    DO_ACTION(MORPHOLOGICAL_CLOSE, closeOpr());
}

void MainWindow::processThinning() {
    DO_ACTION(MORPHOLOGICAL_THINNING, thinning());
}

void MainWindow::processNeighborAve(int value) {
    DO_ACTION(NEIGHBOR_AVERAGING, neighborAve(value));
}

void MainWindow::processNeighborMed(int value) {
    DO_ACTION(NEIGHBOR_MEDIAN, neighborMed(value));
}

void MainWindow::processNeighborGaussian(int value) {
    int r = neighborGaussianRSlider->value();
    double s = neighborGaussianSSlider->value() * 1.0 / 100.0;
    DO_ACTION(NEIGHBOR_GAUSSIAN, neighborGaussian(r, s));
}

void MainWindow::processSobel() {
    DO_ACTION(NEIGHBOR_SOBEL, sobel());
}

void MainWindow::processRoberts() {
    DO_ACTION(NEIGHBOR_ROBERT, roberts());
}

void MainWindow::processCanny() {
    DO_ACTION(NEIGHBOR_CANNY, canny());
}

void MainWindow::processHaze() {
}

/* *
 * Auxilary functions.
 * */

void MainWindow::clearStack() {
    while (images.size() - 1 > cntImageNum)
        images.pop_back();
    cntImageNum ++;
}

void MainWindow::addPreview() {
    if (previewImage != NULL) {
        clearStack();
        images.push_back(*previewImage);
        delete previewImage;
        previewImage = NULL;
    }
}

bool MainWindow::isSlideAction(ACTION_TYPE type) {
    return (type == POINT_THRESHOLD ||
            type == GEOMETRICAL_ROTATION ||
            type == GEOMETRICAL_SCALING ||
            type == GEOMETRICAL_PERSPECTIVE ||
            type == NEIGHBOR_AVERAGING ||
            type == NEIGHBOR_MEDIAN ||
            type == NEIGHBOR_GAUSSIAN);
}

void MainWindow::recordAct(ACTION_TYPE type) {
    if (type == lastActType && isSlideAction(type)) return;
    addPreview();
    originImage = &images[cntImageNum];
    previewImage = new QImage;
    lastActType = type;
    imageEditor.setImage(originImage, previewImage);
}

void MainWindow::afterAct(ACTION_TYPE type) {
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
}
