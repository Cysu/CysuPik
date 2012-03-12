#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pic"));
    showMaximized();

    cntImageNum = -1;
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

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(processMenu);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (cntImageNum > 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum - 1]));
        imageLabel->adjustSize();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum]));
    imageLabel->adjustSize();
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
    if (cntImageNum > 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum - 1]));
        imageLabel->adjustSize();
        cntImageNum --;
        displayHistogramPanel();
    }
}

void MainWindow::redo() {
    if (cntImageNum < images.size() - 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum + 1]));
        imageLabel->adjustSize();
        cntImageNum ++;
        displayHistogramPanel();
    }
}

void MainWindow::displayHistogramPanel() {
    int* tmpH = new int[256];
    ImageEditor::getHistogram(&images[cntImageNum], tmpH);
    delete histogramPanel;
    histogramPanel = new FloatPanel(tr("Histogram"), new Chart(tmpH));
    addDockWidget(Qt::RightDockWidgetArea, histogramPanel);
}

void MainWindow::displayThresholdPanel() {
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(255);
    slider->setFixedSize(256, 20);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(processThresholdMove(int)));
    connect(slider, SIGNAL(sliderPressed()), this, SLOT(processThresholdBegin()));
    connect(slider, SIGNAL(sliderReleased()), this, SLOT(processThresholdEnd()));
    delete thresholdPanel;
    thresholdPanel = new FloatPanel(tr("Threshold"), slider);
    addDockWidget(Qt::RightDockWidgetArea, thresholdPanel);
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

void MainWindow::processThresholdMove(int value) {
    imageEditor.setImage(&images[cntImageNum - 1], &images[cntImageNum]);
    imageEditor.threshold(255 - value);
    imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum]));
    imageLabel->adjustSize();
}

void MainWindow::processThresholdBegin() {
    QImage image = images[cntImageNum];
    clearStack();
    images.push_back(image);
}

void MainWindow::processThresholdEnd() {
    cntImageNum --;
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

/* *
 * Auxilary functions.
 * */

void MainWindow::clearStack() {
    while (images.size() - 1 > cntImageNum)
        images.pop_back();
    cntImageNum ++;
}
