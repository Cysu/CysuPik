#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createPanels();
    createActions();
    createMenus();

    setWindowTitle(tr("Cysu Pik"));
    showMaximized();

    cntImageNum = -1;
    originImage = NULL;
    previewImage = NULL;
    lastActType = NOTHING;

    isPressingShift = false;

    isMarkingup = false;
    isStructuring = false;
    markupRegion = NULL;
    structRegion = NULL;
    markupImage = NULL;
    markupImage_bak = NULL;
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
    inpaintingPanel = NULL;

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

    doInpaintingButton = NULL;
    doMarkupButton = NULL;
    doStructureButton = NULL;
    doVerifyButton = NULL;
}

void MainWindow::createActions() {
    CREATE_ACTION(openAct, "&Open", "Ctrl+O", open);
    CREATE_ACTION(saveasAct, "&Save As", "Ctrl+S", saveas);
    CREATE_ACTION(undoAct, "&Undo", "Ctrl+Z", undo);
    CREATE_ACTION(redoAct, "&Redo", "Ctrl+Y", redo);
    CREATE_ACTION(showHistogramAct, "Show Histogram", "", displayHistogramPanel);
    CREATE_ACTION(convertToGrayscaleAct, "Convert To Grayscale", "Ctrl+G", processConvertToGrayscale);
    CREATE_ACTION(antiColorAct, "Anti Color", "", processAntiColor);
    CREATE_ACTION(thresholdAct, "Threshold", "", displayThresholdPanel);
    CREATE_ACTION(histogramEqualizationAct, "Histogram Equalization", "Ctrl+H", processHistogramEqualization);
    CREATE_ACTION(additionAct, "Addition", "", processAddition);
    CREATE_ACTION(subtractionAct, "Subtraction", "", processSubtraction);
    CREATE_ACTION(translationAct, "Translation", "", processTranslation);
    CREATE_ACTION(horizontalMirrorAct, "Horizontal Mirror", "", processHorizontalMirror);
    CREATE_ACTION(verticalMirrorAct, "Vertical Mirror", "", processVerticalMirror);
    CREATE_ACTION(scalingAct, "Scaling", "", displayScalingPanel);
    CREATE_ACTION(rotationAct, "Rotation", "", displayRotationPanel);
    CREATE_ACTION(perspectiveAct, "Perspective", "", displayPerspectivePanel);
    CREATE_ACTION(erosionAct, "Erosion", "", processErosion);
    CREATE_ACTION(dilationAct, "Dilation", "", processDilation);
    CREATE_ACTION(openOprAct, "Open Operation", "", processOpenOpr);
    CREATE_ACTION(closeOprAct, "Close Operation", "", processCloseOpr);
    CREATE_ACTION(thinningAct, "Thinning", "", processThinning);
    CREATE_ACTION(neighborAveAct, "Neighborhood Averaging", "", displayNeighborAvePanel);
    CREATE_ACTION(neighborMedAct, "Neighborhood Median", "", displayNeighborMedPanel);
    CREATE_ACTION(neighborGaussianAct, "Neighborhood Gaussian", "", displayNeighborGaussianPanel);
    CREATE_ACTION(sobelAct, "Sobel", "", processSobel);
    CREATE_ACTION(robertsAct, "Roberts", "", processRoberts);
    CREATE_ACTION(cannyAct, "Canny", "", processCanny);
    CREATE_ACTION(inpaintingAct, "Inpainting", "Ctrl+I", displayInpaintingPanel);
}

void MainWindow::createMenus() {
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveasAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    processMenu = new QMenu(tr("&Process"), this);
//    pointOperationMenu = processMenu->addMenu(tr("&Point Operation"));
//    pointOperationMenu->addAction(showHistogramAct);
//    pointOperationMenu->addAction(convertToGrayscaleAct);
//    pointOperationMenu->addAction(antiColorAct);
//    pointOperationMenu->addAction(thresholdAct);
//    pointOperationMenu->addAction(histogramEqualizationAct);
//    algOperationMenu = processMenu->addMenu(tr("&Alegrabic Operation"));
//    algOperationMenu->addAction(additionAct);
//    algOperationMenu->addAction(subtractionAct);
//    algOperationMenu->addAction(translationAct);
//    geoOperationMenu = processMenu->addMenu(tr("&Geometrical Operation"));
//    geoOperationMenu->addAction(horizontalMirrorAct);
//    geoOperationMenu->addAction(verticalMirrorAct);
//    geoOperationMenu->addAction(scalingAct);
//    geoOperationMenu->addAction(rotationAct);
//    geoOperationMenu->addAction(perspectiveAct);
//    morphOperationMenu = processMenu->addMenu(tr("&Morphological Operation"));
//    morphOperationMenu->addAction(erosionAct);
//    morphOperationMenu->addAction(dilationAct);
//    morphOperationMenu->addAction(openOprAct);
//    morphOperationMenu->addAction(closeOprAct);
//    morphOperationMenu->addAction(thinningAct);
//    neighborOperationMenu = processMenu->addMenu(tr("&Neighborhood Operation"));
//    neighborOperationMenu->addAction(neighborAveAct);
//    neighborOperationMenu->addAction(neighborMedAct);
//    neighborOperationMenu->addAction(neighborGaussianAct);
//    neighborOperationMenu->addAction(sobelAct);
//    neighborOperationMenu->addAction(robertsAct);
//    neighborOperationMenu->addAction(cannyAct);
    otherOperationMenu = processMenu->addMenu(tr("&Other Operation"));
    otherOperationMenu->addAction(inpaintingAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(processMenu);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Shift:
        isPressingShift = true;
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Shift:
        isPressingShift = false;
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (isMarkingup) {
        prevMouseMovePos = prevMousePressPos = event->pos()-imageLabel->pos()-mainPanel->pos();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (originImage != NULL) {
            imageLabel->setPixmap(QPixmap::fromImage(*originImage));
            imageLabel->adjustSize();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (isMarkingup) {
        prevMouseMovePos.setX(-1);
        prevMouseMovePos.setY(-1);
        if (isPressingShift) {
            QPoint cntPos = event->pos()-imageLabel->pos()-mainPanel->pos();
            __inpainting_drawline(markupImage, prevMousePressPos, cntPos, 0, false);
        }
        structPixelsArray.push_back(structPixels);
        structPixels.clear();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (previewImage != NULL) {
            imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
            imageLabel->adjustSize();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isMarkingup) {
        QPoint cntPos = event->pos()-imageLabel->pos()-mainPanel->pos();
        QPoint prevPos = isPressingShift ? prevMousePressPos : prevMouseMovePos;
        QImage* img;
        if (!isPressingShift) {
            img = markupImage;
        } else {
            img = new QImage;
            *img = markupImage->copy(0, 0, markupImage->width(), markupImage->height());
        }

        int r = isStructuring ? 0 : 6;
        if (prevPos != QPoint(-1, -1))
            __inpainting_drawline(img, prevPos, cntPos, r, isPressingShift);

        if (!isPressingShift) {
            prevMouseMovePos = cntPos;
        } else {
            delete img;
        }
    }
}

void MainWindow::refresh() {
    imageLabel->setPixmap(QPixmap::fromImage(*previewImage));
    imageLabel->adjustSize();
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

void MainWindow::saveas() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), QDir::currentPath(), tr("BMP (*.bmp);;PNG (*.png);;JPG (*.jpg)"));
    if (!fileName.isEmpty()) {
        if (previewImage != NULL)
            previewImage->save(fileName, fileName.mid(fileName.length()-3).toStdString().c_str());
        else
            originImage->save(fileName, fileName.mid(fileName.length()-3).toStdString().c_str());
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

void MainWindow::displayInpaintingPanel() {
    delete doInpaintingButton;
    delete doMarkupButton;
    delete doStructureButton;
    delete doVerifyButton;

    doInpaintingButton = new QPushButton("Inpaiting", this);
    doMarkupButton = new QPushButton("Markup", this);
    doStructureButton = new QPushButton("Structure", this);
    doVerifyButton = new QPushButton("Verify", this);

    doInpaintingButton->setDisabled(true);
    doStructureButton->setDisabled(true);
    doVerifyButton->setDisabled(true);

    connect(doMarkupButton, SIGNAL(clicked()), this, SLOT(processInpaintingMarkup()));
    connect(doInpaintingButton, SIGNAL(clicked()), this, SLOT(processInpainting()));
    connect(doStructureButton, SIGNAL(clicked()), this, SLOT(processInpaintingStructure()));
    connect(doVerifyButton, SIGNAL(clicked()), this, SLOT(processInpaintingVerify()));

    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(doMarkupButton, 0, 0);
    layout->addWidget(doInpaintingButton, 0, 1);
    layout->addWidget(doStructureButton, 1, 0);
    layout->addWidget(doVerifyButton, 1, 1);
    widget->setLayout(layout);

    delete inpaintingPanel;
    inpaintingPanel = new FloatPanel(tr("Inpainting"), widget);
    addDockWidget(Qt::RightDockWidgetArea, inpaintingPanel);
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

void MainWindow::processAddition() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File to Add"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage* image = new QImage(fileName);
        if (image->isNull()) {
            QMessageBox::information(this, tr("Cysu Pic"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        DO_ACTION(ALGEBRAIC_ADDITION, add(image));
    }
}

void MainWindow::processSubtraction() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File to Subtract"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage* image = new QImage(fileName);
        if (image->isNull()) {
            QMessageBox::information(this, tr("Cysu Pic"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        DO_ACTION(ALGEBRAIC_SUBTRACTION, sub(image));
    }
}

void MainWindow::processTranslation() {
    DO_ACTION(ALGEBRAIC_TRANSLATION, translation());
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

void MainWindow::processInpaintingVerify() {
    doMarkupButton->setEnabled(true);
    doVerifyButton->setDisabled(true);
    addPreview();
    originImage = &images[cntImageNum];
}

void MainWindow::processInpainting() {
    isMarkingup = isStructuring = false;
    doMarkupButton->setDisabled(true);
    doStructureButton->setDisabled(true);
    doInpaintingButton->setDisabled(true);
    recordAct(OTHER_INPAINTING);

    inpaintingThread = new InpaintingThread(originImage, previewImage, markupRegion, structRegion, &structPixelsArray, this);
    connect(inpaintingThread, SIGNAL(partDone()), this, SLOT(refresh()));
    connect(inpaintingThread, SIGNAL(finished()), inpaintingThread, SLOT(deleteLater()));
    connect(inpaintingThread, SIGNAL(finished()), this, SLOT(processInpaintingFinish()));
    inpaintingThread->start();
}

void MainWindow::processInpaintingFinish() {
    afterAct(OTHER_INPAINTING);
    doVerifyButton->setEnabled(true);
}

void MainWindow::processInpaintingMarkup() {
    if (isMarkingup) {
        doMarkupButton->setText(tr("Start Markup"));
        doMarkupButton->setDisabled(true);
        doStructureButton->setEnabled(true);

        delete markupImage_bak;
        markupImage_bak = new QImage;
        *markupImage_bak = markupImage->copy(0, 0, markupImage->width(), markupImage->height());
    } else {
        doMarkupButton->setText(tr("End Markup"));
        doStructureButton->setText(tr("Start Structure"));
        doStructureButton->setDisabled(true);
        doInpaintingButton->setDisabled(true);
        doVerifyButton->setDisabled(true);

        isMarkingup = true;
        int w = originImage->width(), h = originImage->height();

        delete markupRegion;
        markupRegion = new bool[w*h];
        memset(markupRegion, false, w*h*sizeof(bool));

        delete markupImage;
        markupImage = new QImage;
        *markupImage = originImage->copy(0, 0 , w, h);
        imageLabel->setPixmap(QPixmap::fromImage(*markupImage));
        imageLabel->adjustSize();
    }
}

void MainWindow::processInpaintingStructure() {
    if (isStructuring) {
        doStructureButton->setText(tr("Start Structure"));
        doInpaintingButton->setEnabled(true);
        doMarkupButton->setEnabled(true);

        isStructuring = false;
        isMarkingup = false;
    } else {
        doStructureButton->setText(tr("End Structure"));
        doInpaintingButton->setDisabled(true);
        doVerifyButton->setDisabled(true);

        isStructuring = true;
        int w = originImage->width(), h = originImage->height();

        delete structRegion;
        structRegion = new bool[w*h];
        memset(structRegion, false, w*h*sizeof(bool));

        structPixels.clear();
        structPixelsArray.clear();

        delete markupImage;
        markupImage = new QImage;
        *markupImage = markupImage_bak->copy(0, 0 , w, h);
        imageLabel->setPixmap(QPixmap::fromImage(*markupImage));
        imageLabel->adjustSize();
    }
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

void MainWindow::__inpainting_drawline(QImage *img, QPoint &p1, QPoint &p2, int r, bool isPreview) {
    int w = originImage->width(), h = originImage->height();
    QPoint dp = p2 - p1;
    if (abs(dp.x()) < abs(dp.y())) {
        int sign = dp.y() > 0 ? 1 : -1;
        for (int dt = 0; dt <= abs(dp.y()); dt ++) {
            int y = p1.y() + sign*dt;
            double k = dt * 1.0 / abs(dp.y());
            int x = (int)(p1.x() + k * dp.x() + 0.5);
            for (int dx = -r; dx <= r; dx ++)
                for (int dy = -r; dy <= r; dy ++) {
                    if (SQR(dx) + SQR(dy) > SQR(r)) continue;
                    if (!ImageEditor::isValidPixel(x+dx, y+dy, w, h)) continue;
                    QPoint pos(x+dx, y+dy);
                    if (isStructuring) {
                        img->setPixel(pos, QColor(Qt::cyan).rgb());
                        if (!isPreview) {
                            structRegion[pos.x()*h + pos.y()] = true;
                            structPixels.push_back(pos);
                        }
                    } else {
                        img->setPixel(pos, QColor(Qt::blue).rgb());
                        markupRegion[pos.x()*h + pos.y()] = true;
                    }
                }
        }
    } else {
        int sign = dp.x() > 0 ? 1 : -1;
        for (int dt = 0; dt <= abs(dp.x()); dt ++) {
            int x = p1.x() + sign*dt;
            double k = dt * 1.0 / abs(dp.x());
            int y = (int)(p1.y() + k * dp.y() + 0.5);
            for (int dx = -r; dx <= r; dx ++)
                for (int dy = -r; dy <= r; dy ++) {
                    if (SQR(dx) + SQR(dy) > SQR(r)) continue;
                    if (!ImageEditor::isValidPixel(x+dx, y+dy, w, h)) continue;
                    QPoint pos(x+dx, y+dy);
                    if (isStructuring) {
                        img->setPixel(pos, QColor(Qt::cyan).rgb());
                        if (!isPreview) {
                            structRegion[pos.x()*h + pos.y()] = true;
                            structPixels.push_back(pos);
                        }
                    } else {
                        img->setPixel(pos, QColor(Qt::blue).rgb());
                        markupRegion[pos.x()*h + pos.y()] = true;
                    }
                }
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*img));
}
