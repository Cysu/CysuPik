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

    histogramEqualizationAct = new QAction(tr("&Histogram Equalization"), this);
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
    pointOperationMenu->addAction(histogramEqualizationAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(processMenu);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    int t = images.size();
    if (t > 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[t - 2]));
        imageLabel->adjustSize();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    int t = images.size();
    if (t > 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[t - 1]));
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
        processHistogramDisplay();
    }
}

void MainWindow::undo() {
    if (cntImageNum > 0) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum - 1]));
        imageLabel->adjustSize();
        cntImageNum --;
        processHistogramDisplay();
    }
}

void MainWindow::redo() {
    if (cntImageNum < images.size() - 1) {
        imageLabel->setPixmap(QPixmap::fromImage(images[cntImageNum + 1]));
        imageLabel->adjustSize();
        cntImageNum ++;
        processHistogramDisplay();
    }
}

void MainWindow::processHistogramDisplay() {
    int* tmpH = new int[256];
    ImageEditor::getHistogram(&images[cntImageNum], tmpH);
    delete histogramPanel;
    histogramPanel = new FloatPanel(tr("Histogram"), new Chart(tmpH));
    addDockWidget(Qt::RightDockWidgetArea, histogramPanel);
}

void MainWindow::processHistogramEqualization() {
    QImage image;
    imageEditor.setImage(&images[cntImageNum], &image);
    clearStack();
    imageEditor.histogramEqualization();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    processHistogramDisplay();
}

/* *
 * Auxilary functions.
 * */

void MainWindow::clearStack() {
    while (images.size() > cntImageNum + 1)
        images.pop_back();
    cntImageNum ++;
}
