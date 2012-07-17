#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <QColor>
#include <iostream>
#include <cmath>
#include <ctime>
#include "utils.h"
#include "const.h"

using namespace std;

const QColor WHITE(255, 255, 255);
const QColor BLACK(0, 0, 0);

class ImageEditor {
public:
    ImageEditor();

    static void getHistogram(QImage* image, int* histogram);
    inline static bool isValidPixel(int x, int y, int w, int h) {
        return (0 <= x && x < w && 0 <= y && y < h);
    }
    inline static double colorDiff(QColor c1, QColor c2) {
        return SQR(c1.red()-c2.red()) + SQR(c1.green()-c2.green()) + SQR(c1.blue()-c2.blue());
    }

    void setImage(QImage* srcImage, QImage* dstImage);

    void convertToGrayscale();
    void antiColor();
    void threshold(int value);
    void histogramEqualization();
    void add(QImage* image);
    void sub(QImage* image);
    void translation();
    void horizontalMirror();
    void verticalMirror();
    void scaling(int hValue, int vValue);
    void rotation(int value);
    void perspective(int xValue, int yValue, int zValue);
    void erosion();
    void dilation();
    void openOpr();
    void closeOpr();
    void thinning();
    void neighborAve(int value);
    void neighborMed(int value);
    void neighborGaussian(int r, double s);
    void sobel();
    void roberts();
    void canny();
    void inpainting(bool* markupRegion);

private:
    QImage* srcImage;
    QImage* dstImage;

    void __sort(vector<int>& a, int l, int r);
    void __canny_threshold(int x, int y, int* &mark, int th, int tl);
    void __get_gradient(QImage* img, double* gx, double* gy);
    void __get_gradient(bool* binImg, int w, int h, double* gx, double* gy);
};

#endif // IMAGEEDITOR_H
