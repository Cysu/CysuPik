#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pic"));
    showMaximized();

    cntImageNum = -1;
    previewImage = NULL;
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
    horizontalScalingPanel = NULL;
    verticalScalingPanel = NULL;
    rotationPanel = NULL;
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Y"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    antiColorAct = new QAction(tr("Anti Color"), this);
    antiColorAct->setShortcut(tr("Ctrl+I"));
    connect(antiColorAct, SIGNAL(triggered()), this, SLOT(processAntiColor()));

    thresholdAct = new QAction(tr("Threshold"), this);
    thresholdAct->setShortcut(tr("Ctrl+H"));
    connect(thresholdAct, SIGNAL(triggered()), this, SLOT(displayThresholdPanel()));

    histogramEqualizationAct = new QAction(tr("Histogram Equalization"), this);
    histogramEqualizationAct->setShortcut(tr("Ctrl+B"));
    connect(histogramEqualizationAct, SIGNAL(triggered()), this, SLOT(processHistogramEqualization()));

    horizontalMirrorAct = new QAction(tr("Horizontal Mirror"), this);
    horizontalMirrorAct->setShortcut(tr("Ctrl+L"));
    connect(horizontalMirrorAct, SIGNAL(triggered()), this, SLOT(processHorizontalMirror()));

    verticalMirrorAct = new QAction(tr("Vertical Mirror"), this);
    verticalMirrorAct->setShortcut(tr("Ctrl+K"));
    connect(verticalMirrorAct, SIGNAL(triggered()), this, SLOT(processVerticalMirror()));

    scalingAct = new QAction(tr("Scaling"), this);
    scalingAct->setShortcut(tr("Ctrl+S"));
    connect(scalingAct, SIGNAL(triggered()), this, SLOT(displayScalingPanel()));

    rotationAct = new QAction(tr("Rotation"), this);
    rotationAct->setShortcut(tr("Ctrl+R"));
    connect(rotationAct, SIGNAL(triggered()), this, SLOT(displayRotationPanel()));

    hazeAct = new QAction(tr("Haze"), this);
    hazeAct->setShortcut(tr("Ctrl+E"));
    connect(hazeAct, SIGNAL(triggered()), this, SLOT(processHaze()));
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
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(1);
    slider->setMaximum(200);
    slider->setFixedSize(200, 20);
    slider->setValue(100);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(processHorizontalScaling(int)));
    delete horizontalScalingPanel;
    horizontalScalingPanel = new FloatPanel(tr("Horizontal Scaling"), slider);
    addDockWidget(Qt::TopDockWidgetArea, horizontalScalingPanel);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(1);
    slider->setMaximum(200);
    slider->setFixedSize(200, 20);
    slider->setValue(100);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(processVerticalScaling(int)));
    delete verticalScalingPanel;
    verticalScalingPanel = new FloatPanel(tr("Vertical Scaling"), slider);
    addDockWidget(Qt::TopDockWidgetArea, verticalScalingPanel);
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

void MainWindow::processAntiColor() {
    QImage image = images[cntImageNum];
    image.invertPixels();
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processThreshold(int value) {
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.threshold(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processHistogramEqualization() {
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
    QImage image = images[cntImageNum].mirrored(true, false);
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processVerticalMirror() {
    QImage image = images[cntImageNum].mirrored(false, true);
    clearStack();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    displayHistogramPanel();
}

void MainWindow::processHorizontalScaling(int value) {
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.horizontalScaling(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processVerticalScaling(int value) {
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.verticalScaling(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processRotation(int value) {
    if (previewImage == NULL) previewImage = new QImage;
    imageEditor.setImage(&images[cntImageNum], previewImage);
    imageEditor.rotation(value);
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
    displayHistogramPanel(previewImage);
}

void MainWindow::processHaze() {
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
