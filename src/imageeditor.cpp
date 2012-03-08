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

void ImageEditor::histogramEqualization() {
    if (srcImage->isGrayscale()) {
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
    } else {
        int histogram[3][256];
        memset(histogram, 0, sizeof(histogram));
        for (int i = 0; i < srcImage->width(); i ++)
            for (int j = 0; j < srcImage->height(); j ++) {
                QColor c(srcImage->pixel(i, j));
                int r, g, b;
                c.getRgb(&r, &g, &b);
                histogram[0][r] ++;
                histogram[1][g] ++;
                histogram[2][b] ++;
            }

        int cdf[3][256];
        for (int i = 0; i < 3; i ++) cdf[i][0] = histogram[i][0];
        int cdfMin[3];
        for (int i = 0; i < 3; i ++) cdfMin[i] = srcImage->height() * srcImage->width();
        for (int i = 0; i < 3; i ++)
            if (cdf[i][0] > 0 && cdfMin[i] > cdf[i][0]) cdfMin[i] = cdf[i][0];
        for (int i = 1; i < 256; i ++) {
            for (int j = 0; j < 3; j ++) {
                cdf[j][i] = cdf[j][i - 1] + histogram[j][i];
                if (cdf[j][i] > 0 && cdfMin[j] > cdf[j][i]) cdfMin[j] = cdf[j][i];
            }
        }

        int f[3][256];
        memset(f, 0, sizeof(f));
        for (int i = 0; i < 256; i ++) {
            for (int j = 0; j < 3; j ++) {
                f[j][i] = (int)((cdf[j][i] - cdfMin[j]) * 255.0 / (srcImage->width() * srcImage->height() - cdfMin[j]) + 0.5);
            }
        }

        *dstImage = srcImage->copy(0, 0, srcImage->width(), srcImage->height());
        for (int i = 0; i < srcImage->width(); i ++)
            for (int j = 0; j < srcImage->height(); j ++) {
                QColor c(srcImage->pixel(i, j));
                int r, g, b;
                c.getRgb(&r, &g, &b);
                r = f[0][r];
                g = f[1][g];
                b = f[2][b];
                c.setRgb(r, g, b);
                dstImage->setPixel(i, j, c.rgb());
            }
    }
}
