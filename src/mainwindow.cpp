#include "mainwindow.h"

#define CREATE_ACTION(act, text, shortcut, func) \
    act = new QAction(tr(text), this); \
    act->setShortcut(tr(shortcut)); \
    connect(act, SIGNAL(triggered()), this, SLOT(func()));

#define RECORD_EDIT(type) \
    if (prevEditType != type) addPreview(); \
    prevEditType = type;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pic"));
    showMaximized();

    cntImageNum = -1;
    previewImage = NULL;
    prevEditType = NOTHING;
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
}

void MainWindow::createActions() {
    CREATE_ACTION(openAct, "&Open", "Ctrl+O", open);
    CREATE_ACTION(undoAct, "&Undo", "Ctrl+Z", undo);
    CREATE_ACTION(redoAct, "&Redo", "Ctrl+Y", redo);
    CREATE_ACTION(antiColorAct, "Anti Color", "Ctrl+I", processAntiColor);
    CREATE_ACTION(thresholdAct, "Threshold", "Ctrl+H", displayThresholdPanel);
    CREATE_ACTION(histogramEqualizationAct, "Histogram Equalization", "Ctrl+B", processHistogramEqualization);
    CREATE_ACTION(horizontalMirrorAct, "Horizontal Mirror", "Ctrl+L", processHorizontalMirror);
    CREATE_ACTION(verticalMirrorAct, "Vertical Mirror", "Ctrl+K", processVerticalMirror);
    CREATE_ACTION(scalingAct, "Scaling", "Ctrl+S", displayScalingPanel);
    CREATE_ACTION(rotationAct, "Rotation", "Ctrl+R", displayRotationPanel);
    CREATE_ACTION(perspectiveAct, "Perspective", "Ctrl+P", displayPerspectivePanel);
    CREATE_ACTION(hazeAct, "Haze", "Ctrl+E", processHaze);
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    processMenu = new QMenu(tr("&Process"), this);
    pointOperationMenu = processMenu->addMenu(tr("&Point Operation"));
    pointOperationMenu->addAction(antiColorAct);
    pointOperationMenu->addAction(thresholdAct);
    pointOperationMenu->addAction(histogramEqualizationAct);
    geoOperationMenu = processMenu->addMenu(tr("&Geometric Operation"));
    geoOperationMenu->addAction(horizontalMirrorAct);
    geoOperationMenu->addAction(verticalMirrorAct);
    geoOperationMenu->addAction(scalingAct);
    geoOperationMenu->addAction(rotationAct);
    geoOperationMenu->addAction(perspectiveAct);
    otherOperationMenu = processMenu->addMenu(tr("&Other Operation"));
    otherOperationMenu->addAction(hazeAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(processMenu);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (cntImageNum > 0) {
            imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum - 1]));
            imageLabel->adjustSize();
        }
    } else if (event->button() == Qt::RightButton) {
        addPreview();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum]));
        imageLabel->adjustSize();
    }
}

/* *
 * Action response.
 * */

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Cysu Pic"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        clearStack();
        images.push_back(image);
        imageLabel->setPixmap(QPixmap::fromImage(image));
        imageLabel->adjustSize();
        displayHistogramPanel();
        prevEditType = NOTHING;
    }
}

void MainWindow::undo() {
    if (previewImage != NULL && cntImageNum >= 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum]));
        imageLabel->adjustSize();
        displayHistogramPanel();
    } else if (previewImage == NULL && cntImageNum > 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[-- cntImageNum]));
        imageLabel->adjustSize();
        displayHistogramPanel();
    }
}

void MainWindow::redo() {
    if (previewImage != NULL) {
        imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
        imageLabel->adjustSize();
        displayHistogramPanel(previewImage);
    } else if (cntImageNum < images.size() - 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[++ cntImageNum]));
        imageLabel->adjustSize();
        displayHistogramPanel();
    }
}

void MainWindow::displayHistogramPanel(QImage* image) {
    int* tmpH = new int[256];
    if (image == NULL)
        ImageEditor::getHistogram(&images[cntImageNum], tmpH);
    else
        ImageEditor::getHistogram(image, tmpH);
    delete histogramPanel;
    histogramPanel = new FloatPanel(tr("Histogram"), new Chart(tmpH));
    addDockWidget(Qt::RightDockWidgetArea, histogramPanel);
}

void MainWindow::displayThresholdPanel() {
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(255);
    slider->setFixedSize(256, 20);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(processThreshold(int)));
    delete thresholdPanel;
    thresholdPanel = new FloatPanel(tr("Threshold"), slider);
    addDockWidget(Qt::TopDockWidgetArea, thresholdPanel);
}

void MainWindow::displayScalingPanel() {
    QSlider* horizontalScalingSlider = new QSlider(Qt::Horizontal, this);
    horizontalScalingSlider->setMinimum(1);
    horizontalScalingSlider->setMaximum(200);
    horizontalScalingSlider->setFixedSize(200, 20);
    horizontalScalingSlider->setValue(100);
    connect(horizontalScalingSlider, SIGNAL(valueChanged(int)), this, SLOT(processHorizontalScaling(int)));

    QSlider* verticalScalingSlider = new QSlider(Qt::Horizontal, this);
    verticalScalingSlider->setMinimum(1);
    verticalScalingSlider->setMaximum(200);
    verticalScalingSlider->setFixedSize(200, 20);
    verticalScalingSlider->setValue(100);
    connect(verticalScalingSlider, SIGNAL(valueChanged(int)), this, SLOT(processVerticalScaling(int)));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    QLabel* horizontalScalingLabel = new QLabel(tr("Horizontal"));
    QLabel* verticalScalingLabel = new QLabel(tr("Vertical"));
    layout->addWidget(horizontalScalingLabel, 0, 0);
    layout->addWidget(horizontalScalingSlider, 0, 1);
    layout->addWidget(verticalScalingLabel, 1, 0);
    layout->addWidget(verticalScalingSlider, 1, 1);
    widget->setLayout(layout);

    delete scalingPanel;
    scalingPanel = new FloatPanel(tr("Scaling"), widget);
    addDockWidget(Qt::TopDockWidgetArea, scalingPanel);
}

void MainWindow::displayRotationPanel() {
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(360);
    slider->setFixedSize(361, 20);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(processRotation(int)));
    delete rotationPanel;
    rotationPanel = new FloatPanel(tr("Rotation"), slider);
    addDockWidget(Qt::TopDockWidgetArea, rotationPanel);
}

void MainWindow::displayPerspectivePanel() {
    QSlider* xSlider = new QSlider(Qt::Horizontal, this);
    xSlider->setMinimum(0);
    xSlider->setMaximum(100);
    xSlider->setFixedSize(200, 20);
    xSlider->setValue(0);
    connect(xSlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspectiveX(int)));

    QSlider* ySlider = new QSlider(Qt::Horizontal, this);
    ySlider->setMinimum(0);
    ySlider->setMaximum(100);
    ySlider->setFixedSize(200, 20);
    ySlider->setValue(0);
    connect(ySlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspectiveY(int)));

    QSlider* zSlider = new QSlider(Qt::Horizontal, this);
    zSlider->setMinimum(0);
    zSlider->setMaximum(100);
    zSlider->setFixedSize(200, 20);
    zSlider->setValue(100);
    connect(zSlider, SIGNAL(valueChanged(int)), this, SLOT(processPerspectiveZ(int)));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    QLabel* xLabel = new QLabel(tr("X"));
    QLabel* yLabel = new QLabel(tr("Y"));
    QLabel* zLabel = new QLabel(tr("Z"));
    layout->addWidget(xLabel, 0, 0);
    layout->addWidget(xSlider, 0, 1);
    layout->addWidget(yLabel, 1, 0);
    layout->addWidget(ySlider, 1, 1);
    layout->addWidget(zLabel, 2, 0);
    layout->addWidget(zSlider, 2, 1);
    widget->setLayout(layout);

    delete perspectivePanel;
    perspectivePanel = new FloatPanel(tr("Perspective"), widget);
    addDockWidget(Qt::TopDockWidgetArea, perspectivePanel);
}

void MainWindow::processAntiColor() {
    RECORD_EDIT(NOTHING);
    QImage image = images[cntImageNum];
    image.invertPixels();
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processThreshold(int value) {
    RECORD_EDIT(THRESHOLD);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.threshold(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processHistogramEqualization() {
    RECORD_EDIT(NOTHING);
    QImage image;
    imageEditor.setImage(&images[cntImageNum], &image);
    clearStack();
    imageEditor.histogramEqualization();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processHorizontalMirror() {
    RECORD_EDIT(NOTHING);
    QImage image = images[cntImageNum].mirrored(true, false);
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processVerticalMirror() {
    RECORD_EDIT(NOTHING);
    QImage image = images[cntImageNum].mirrored(false, true);
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processHorizontalScaling(int value) {
    RECORD_EDIT(HORIZONTAL_SCALING);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.horizontalScaling(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processVerticalScaling(int value) {
    RECORD_EDIT(VERTICAL_SCALING);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.verticalScaling(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processRotation(int value) {
    RECORD_EDIT(ROTATION);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.rotation(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processPerspectiveX(int value) {
    RECORD_EDIT(PERSPECTIVE_X);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.perspectiveX(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processPerspectiveY(int value) {
    RECORD_EDIT(PERSPECTIVE_Y);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.perspectiveY(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processPerspectiveZ(int value) {
    RECORD_EDIT(PERSPECTIVE_Z);
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.perspectiveZ(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processHaze() {
    RECORD_EDIT(NOTHING);
    QImage image;
    imageEditor.setImage(&images[cntImageNum], &image);
    clearStack();
    imageEditor.haze();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
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
