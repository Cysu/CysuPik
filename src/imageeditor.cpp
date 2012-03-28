#include "imageeditor.h"
#include <cstdio>

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

void ImageEditor::haze() {
    int w = srcImage->width(), h = srcImage->height();
    double** t = new double*[w];
    int** tmp = new int*[w];
    int** darkChannel = new int*[w];
    for (int i = 0; i < w; i ++) {
        t[i] = new double[h];
        tmp[i] = new int[h];
        darkChannel[i] = new int[h];
    }
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            QColor c(srcImage->pixel(i, j));
            tmp[i][j] = min(c.red(), c.green());
            tmp[i][j] = min(tmp[i][j], c.blue());
        }
    // Naive neighbor
    *dstImage = srcImage->copy();
    int A = 0;
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            darkChannel[i][j] = 256;
            for (int dx = -7; dx <= 7; dx ++)
                for (int dy = -7; dy <= 7; dy ++) {
                    if (0 <= i + dx && i + dx < w && 0 <= j + dy && j + dy < h) {
                        darkChannel[i][j] = min(darkChannel[i][j], tmp[i + dx][j + dy]);
                    }
                }
            A = max(A, darkChannel[i][j]);
            QColor c(darkChannel[i][j], darkChannel[i][j], darkChannel[i][j]);
            //dstImage->setPixel(i, j, c.rgb());

        }

    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            t[i][j] = 1 - darkChannel[i][j] * 1.0 / A;
            //printf("%d %d %lf\n", i, j, t[i][j]);
        }

    *dstImage = srcImage->copy();
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            QColor c(srcImage->pixel(i, j));
            if (darkChannel[i][j] == A) continue;
            if (darkChannel[i][j] == min(c.red(), min(c.green(), c.blue()))) continue;

            //printf("%d %lf, %d %d %d, %d %d %d\n", darkChannel[i][j], t[i][j], c.red(), c.green(), c.blue(), (int)((c.red() - darkChannel[i][j]) / t[i][j]), (int)((c.green() - darkChannel[i][j]) / t[i][j]), (int)((c.blue() - darkChannel[i][j]) / t[i][j]));
            int r = (c.red() - darkChannel[i][j]) / t[i][j];
            int g = (c.green() - darkChannel[i][j]) / t[i][j];
            int b = (c.blue() - darkChannel[i][j]) / t[i][j];

            if (r > 255 || g > 255 || b > 255) continue;
            c.setRgb(r, g, b);
            dstImage->setPixel(i, j, c.rgb());
        }
}
