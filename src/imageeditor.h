#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <QColor>
#include <iostream>
#include <cmath>

using namespace std;

enum EDIT_TYPE {
    NOTHING,
    THRESHOLD,
    VERTICAL_SCALING,
    HORIZONTAL_SCALING,
    ROTATION,
    PERSPECTIVE_X,
    PERSPECTIVE_Y,
    PERSPECTIVE_Z
};

class ImageEditor {
public:
    ImageEditor();

    static void getHistogram(QImage* image, int* histogram);

    void setImage(QImage* srcImage, QImage* dstImage);

    void threshold(int value);
    void histogramEqualization();
    void horizontalScaling(int value);
    void verticalScaling(int value);
    void rotation(int value);
    void perspectiveX(int value);
    void perspectiveY(int value);
    void perspectiveZ(int value);
    void haze();

private:
    QImage* srcImage;
    QImage* dstImage;

    void floodfill(int x, int y, double** t, int** darkChannel, bool** mark, int maxD);
};

#endif // IMAGEEDITOR_H
