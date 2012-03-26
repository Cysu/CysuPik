#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <QColor>
#include <iostream>
#include <cmath>

using namespace std;

class ImageEditor {
public:
    ImageEditor();

    static void getHistogram(QImage* image, int* histogram);

    void threshold(int value);
    void histogramEqualization();
    void horizontalScaling(int value);
    void verticalScaling(int value);
    void rotation(int value);
    void setImage(QImage* srcImage, QImage* dstImage);

private:
    QImage* srcImage;
    QImage* dstImage;
};

#endif // IMAGEEDITOR_H
