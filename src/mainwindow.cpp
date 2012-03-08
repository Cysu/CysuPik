#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->resize(0, 0);

    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pic"));
    showMaximized();
}

// SLOTS
void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Cysu Pic"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        images.push_back(image);
        imageLabel->setPixmap(QPixmap::fromImage(image));
        imageLabel->adjustSize();
    }
}

void MainWindow::processHistogramEqualization() {
    QImage image;
    imageEditor.setImage(&images[images.size() - 1], &image);
    imageEditor.histogramEqualization();
    images.push_back(image);
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();

    vector<int> x;
    for (int i = 0; i < 255; i ++) x.push_back(i);
    addDockWidget(Qt::RightDockWidgetArea, new FloatPanel(tr("Histogram"), new Chart(x, imageEditor.getHistogram())));
}

void MainWindow::createPanels() {
    mainPanel = new QScrollArea;
    mainPanel->setBackgroundRole(QPalette::Dark);
    mainPanel->setWidget(imageLabel);
    setCentralWidget(mainPanel);
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    histogramEqualizationAct = new QAction(tr("&Histogram Equalization"), this);
    histogramEqualizationAct->setShortcut(tr("Ctrl+B"));
    connect(histogramEqualizationAct, SIGNAL(triggered()), this, SLOT(processHistogramEqualization()));
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);

    processMenu = new QMenu(tr("&Process"), this);
    pointOperationMenu = processMenu->addMenu(tr("&Point Operation"));
    pointOperationMenu->addAction(histogramEqualizationAct);

    menuBar()->addMenu(fileMenu);
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
