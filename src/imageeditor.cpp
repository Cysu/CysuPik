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

void ImageEditor::perspectiveX(int value) {
    double t = value / 100.0, w = srcImage->width(), h = srcImage->height();
    t /= w;
    //QTransform trans(t, 0, 0, 0, t, 0, w*(1-t)/2, h*(1-t)/2, 1);
    QTransform trans(1, 0, t, 0, 1, 0, 0, 0, 1);

    *dstImage = srcImage->transformed(trans);
}

void ImageEditor::perspectiveY(int value) {
    double t = value / 100.0, w = srcImage->width(), h = srcImage->height();
    t /= w;
    QTransform trans(1, 0, 0, 0, 1, t, 0, 0, 1);

    *dstImage = srcImage->transformed(trans);
}

void ImageEditor::perspectiveZ(int value) {
    double t = value / 100.0, w = srcImage->width(), h = srcImage->height();
    QTransform trans(t, 0, 0, 0, t, 0, w*(1-t)/2, h*(1-t)/2, 1);
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
    int maxD = 0;
    QColor A;
    int patch = 7;
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            darkChannel[i][j] = tmp[i][j];
            QColor a(srcImage->pixel(i, j));
            for (int dx = -patch; dx <= patch; dx ++)
                for (int dy = -patch; dy <= patch; dy ++) {
                    if (0 <= i + dx && i + dx < w && 0 <= j + dy && j + dy < h) {
                        QColor b(srcImage->pixel(i + dx, j + dy));
                        darkChannel[i][j] = min(darkChannel[i][j], tmp[i + dx][j + dy]);
                    }
                }
            if (maxD < darkChannel[i][j]) {
                maxD = darkChannel[i][j];
                A = a;
            }
            QColor c(darkChannel[i][j], darkChannel[i][j], darkChannel[i][j]);
            dstImage->setPixel(i, j, c.rgb());
        }

    *dstImage = srcImage->copy();
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            t[i][j] = 1 - darkChannel[i][j] * 0.95 / maxD;
            QColor c(t[i][j] * 255, t[i][j] * 255, t[i][j] * 255);
            dstImage->setPixel(i, j, c.rgb());
        }

    // myself
    /*bool** mark = new bool*[w];
    for (int i = 0; i < w; i ++) mark[i] = new bool[h];
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) mark[i][j] = false;
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++)
            if (t[i][j] < 0.5 && !mark[i][j]) {
                floodfill(i, j, t, darkChannel, mark, maxD);
            }*/

    /**dstImage = srcImage->copy();
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            QColor c(srcImage->pixel(i, j));
            int d = max(abs(c.red() - c.green()), abs(c.green() - c.blue()));
            d = max(d, abs(c.red() - c.blue()));

            int r = A.red() + (c.red() - A.red()) / max(0.3, t[i][j]);
            int g = A.green() + (c.green() - A.green()) / max(0.3, t[i][j]);
            int b = A.blue() + (c.blue() - A.blue()) / max(0.3, t[i][j]);

            if (r > 255 || g > 255 || b > 255) continue;
            if (r < 0 || g < 0 || b < 0) continue;// printf("%d %d %d\n", r, g, b);
            c.setRgb(r, g, b);
            dstImage->setPixel(i, j, c.rgb());
        }*/
}

void ImageEditor::floodfill(int x, int y, double** t, int** darkChannel, bool** mark, int maxD) {
    for (int dx = -1; dx <= 1; dx ++)
        for (int dy = -1; dy <= 1; dy ++)
            if (0 <= x + dx && x + dx < srcImage->width() && 0 <= y + dy && y + dy < srcImage->height())
                if (!mark[x+dx][y+dy] && t[x+dx][y+dy] >= 0.5) {
                    mark[x+dx][y+dy] = true;
                    QColor a(srcImage->pixel(x, y));
                    QColor b(srcImage->pixel(x+dx, y+dy));
                    int th = 10;
                    if (abs(a.red() - b.red()) < th && abs(a.green() - b.green()) < th && abs(a.blue() - b.blue()) < th) {
                        int d = min(b.red(), b.green());
                        d = min(d, b.blue());
                        t[x+dx][y+dy] = t[x][y] * 1.0 + (1 - d*0.95/maxD) * 0.0;
                        floodfill(x+dx, y+dy, t, darkChannel, mark, maxD);
                    }
                }
}
