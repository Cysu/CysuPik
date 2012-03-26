#include "imageeditor.h"

ImageEditor::ImageEditor() {
}

void ImageEditor::getHistogram(QImage* image, int* histogram) {
    for (int i = 0; i < 256; i ++) histogram[i] = 0;
    for (int i = 0; i < image->width(); i ++)
        for (int j = 0; j < image->height(); j ++) {
            int g = qGray(image->pixel(i, j));
            histogram[g] ++;
        }
}

void ImageEditor::setImage(QImage* srcImage, QImage* dstImage) {
    this->srcImage = srcImage;
    this->dstImage = dstImage;
}

void ImageEditor::threshold(int value) {
    *dstImage = srcImage->copy(0, 0, srcImage->width(), srcImage->height());
    for (int i = 0; i < dstImage->width(); i ++)
        for (int j = 0; j < dstImage->height(); j ++) {
            int g = qGray(dstImage->pixel(i, j));
            if (g > value) dstImage->setPixel(i, j, 0);
            else dstImage->setPixel(i, j, 255);
        }
}

void ImageEditor::histogramEqualization() {
    int histogram[256];
    memset(histogram, 0, sizeof(histogram));
    for (int i = 0; i < srcImage->width(); i ++)
        for (int j = 0; j < srcImage->height(); j ++) {
            int g = qGray(srcImage->pixel(i, j));
            histogram[g] ++;
        }

    int cdf[256];
    cdf[0] = histogram[0];
    int cdfMin = srcImage->height() * srcImage->width();
    if (cdf[0] > 0 && cdfMin > cdf[0]) cdfMin = cdf[0];
    for (int i = 1; i < 256; i ++) {
        cdf[i] = cdf[i - 1] + histogram[i];
        if (cdf[i] > 0 && cdfMin > cdf[i]) cdfMin = cdf[i];
    }

    int f[256];
    memset(f, 0, sizeof(f));
    for (int i = 0; i < 256; i ++) {
        f[i] = (int)((cdf[i] - cdfMin) * 255.0 / (srcImage->width() * srcImage->height() - cdfMin) + 0.5);
    }

    *dstImage = srcImage->copy(0, 0, srcImage->width(), srcImage->height());
    for (int i = 0; i < srcImage->width(); i ++)
        for (int j = 0; j < srcImage->height(); j ++) {
            int g = qGray(srcImage->pixel(i, j));
            dstImage->setPixel(i, j, f[g]);
        }
}

void ImageEditor::horizontalScaling(int value) {
    int w = srcImage->width() / 100.0 * value, h = srcImage->height();
    *dstImage = srcImage->scaled(w, h);
}

void ImageEditor::verticalScaling(int value) {
    int w = srcImage->width(), h = srcImage->height() / 100.0 * value;
    *dstImage = srcImage->scaled(w, h);
}

void ImageEditor::rotation(int value) {
    double theta = value * 3.1415926 / 180.0;
    QTransform trans(cos(theta), sin(theta), -sin(theta), cos(theta), 0, 0);
    *dstImage = srcImage->transformed(trans);
}
