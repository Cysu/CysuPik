#include "imageeditor.h"

ImageEditor::ImageEditor() {

}

void ImageEditor::setImage(QImage* srcImage, QImage* dstImage) {
    this->srcImage = srcImage;
    this->dstImage = dstImage;
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
