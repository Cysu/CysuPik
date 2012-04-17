#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <QColor>
#include <iostream>
#include <cmath>
#include "utils.h"
#include "const.h"

using namespace std;

const QColor WHITE(255, 255, 255);
const QColor BLACK(0, 0, 0);

class ImageEditor {
public:
    ImageEditor();

    static void getHistogram(QImage* image, int* histogram);

    void setImage(QImage* srcImage, QImage* dstImage);

    void antiColor();
    void threshold(int value);
    void histogramEqualization();
    void horizontalMirror();
    void verticalMirror();
    void scaling(int hValue, int vValue);
    void rotation(int value);
    void perspective(int xValue, int yValue, int zValue);
    void haze();

private:
    QImage* srcImage;
    QImage* dstImage;

    void floodfill(int x, int y, double** t, int** darkChannel, bool** mark, int maxD);
};

#endif // IMAGEEDITOR_H
