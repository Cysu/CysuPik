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

void ImageEditor::convertToGrayscale() {
    *dstImage = srcImage->convertToFormat(QImage::Format_Indexed8);
    QVector<QRgb> colors;
    for (int i = 0; i < 256; i ++) colors.push_back(QColor(i,i,i).rgb());
    dstImage->setColorTable(colors);

    int w = srcImage->width(), h = srcImage->height();
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++)
            dstImage->setPixel(i, j, qGray(srcImage->pixel(i, j)));
}

void ImageEditor::antiColor() {
    *dstImage = srcImage->copy(0, 0, srcImage->width(), srcImage->height());
    dstImage->invertPixels();
}

void ImageEditor::threshold(int value) {
    *dstImage = srcImage->copy(0, 0, srcImage->width(), srcImage->height());
    for (int i = 0; i < dstImage->width(); i ++)
        for (int j = 0; j < dstImage->height(); j ++) {
            int g = qGray(dstImage->pixel(i, j));
            if (g < value) dstImage->setPixel(i, j, 0);
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

void ImageEditor::add(QImage *image) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    QImage tmp = image->scaled(w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            int g1 = qGray(srcImage->pixel(i, j));
            int g2 = qGray(tmp.pixel(i, j));
            dstImage->setPixel(i, j, min(255, g1+g2));
        }
}

void ImageEditor::sub(QImage *image) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    QImage tmp = image->scaled(w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            int g1 = qGray(srcImage->pixel(i, j));
            int g2 = qGray(tmp.pixel(i, j));
            dstImage->setPixel(i, j, max(0, g1-g2));
        }
}

void ImageEditor::translation() {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    for (int j = 0; j < h; j ++) dstImage->setPixel(0, j, 0);
    for (int i = 1; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            int g1 = qGray(srcImage->pixel(i, j));
            int g2 = qGray(srcImage->pixel(i-1, j));
            dstImage->setPixel(i, j, max(0, g1-g2));
        }
}

void ImageEditor::horizontalMirror() {
    *dstImage = srcImage->mirrored(true, false);
}

void ImageEditor::verticalMirror() {
    *dstImage = srcImage->mirrored(false, true);
}

void ImageEditor::scaling(int hValue, int vValue) {
    int w = srcImage->width() / 100.0 * hValue, h = srcImage->height() / 100.0 * vValue;
    *dstImage = srcImage->scaled(w, h);
}

void ImageEditor::rotation(int value) {
    double theta = value * 3.1415926 / 180.0;
    QTransform trans(cos(theta), sin(theta), -sin(theta), cos(theta), 0, 0);
    *dstImage = srcImage->transformed(trans);
}

void ImageEditor::perspective(int xValue, int yValue, int zValue) {
    double w = srcImage->width(), h = srcImage->height();
    double m = max(w, h);
    double tx = xValue * 3.1415926 / 180.0, ty = yValue * 3.1415926 / 180.0;
    double zv = (zValue - 100.0) / 100.0 * m;
    double A = -sin(ty) / m, B = -sin(tx) / m;
    double C = 1 + w*sin(ty)/2/m + h*sin(tx)/2/m - zv/m;
    double m11 = cos(ty) + w*A/2, m21 = w*B/2;
    double m12 = h*A/2, m22 = cos(tx) + h*B/2;
    double m31 = w*C/2 - w*cos(ty)/2, m32 = h*C/2 - h*cos(tx)/2;
    if (A == 0 && B == 0) {
        QTransform trans(m11/C, 0, 0, 0, m22/C, 0, 0, 0, 1);
        *dstImage = srcImage->transformed(trans);
    } else {
        QTransform trans(m11, m12, A, m21, m22, B, m31, m32, C);
        *dstImage = srcImage->transformed(trans);
    }

}

void ImageEditor::erosion() {
    histogramEqualization();
    threshold(128);
    int w = dstImage->width(), h = dstImage->height();
    QImage tmp = dstImage->copy(0, 0, w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            if (qGray(tmp.pixel(i, j)) == 0) continue;
            for (int dx = -1; dx <= 1; dx ++)
                for (int dy = -1; dy <= 1; dy ++)
                    if (0 <= i+dx && i+dx < w && 0 <= j+dy && j+dy < h)
                        if (qGray(tmp.pixel(i+dx, j+dy)) == 0) goto ero;
            continue;
        ero:
            dstImage->setPixel(i, j, 0);
        }
}

void ImageEditor::dilation() {
    histogramEqualization();
    threshold(128);
    int w = dstImage->width(), h = dstImage->height();
    QImage tmp = dstImage->copy(0, 0, w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            if (qGray(tmp.pixel(i, j)) == 255) continue;
            for (int dx = -1; dx <= 1; dx ++)
                for (int dy = -1; dy <= 1; dy ++)
                    if (0 <= i+dx && i+dx < w && 0 <= j+dy && j+dy < h)
                        if (qGray(tmp.pixel(i+dx, j+dy)) == 255) goto ero;
            continue;
        ero:
            dstImage->setPixel(i, j, 255);
        }
}

void ImageEditor::openOpr() {
    erosion();
    dilation();    
}

void ImageEditor::closeOpr() {
    dilation();
    erosion();
}

void ImageEditor::thinning() {
    threshold(128);
    int w = srcImage->width(), h = srcImage->height();
    int dx[8] = {0,-1,-1,-1,0,1,1,1};
    int dy[8] = {1,1,0,-1,-1,-1,0,1};
    QImage tmp = dstImage->copy(0, 0, w, h);
    while (true) {
        bool change = false;
        for (int i = 1; i < w-1; i ++)
            for (int j = 1; j < h-1; j ++) {
                if (BIN_VALUE(tmp, i, j) == 0) continue;
                int a = abs(BIN_VALUE(tmp, i+dx[0], j+dy[0]) - BIN_VALUE(tmp, i+dx[7], j+dy[7]));
                int s = BIN_VALUE(tmp, i+dx[7], j+dy[7]);
                for (int k = 0; k < 7; k ++) {
                    a += abs(BIN_VALUE(tmp, i+dx[k], j+dy[k]) - BIN_VALUE(tmp, i+dx[k+1], j+dy[k+1]));
                    s += BIN_VALUE(tmp, i+dx[k], j+dy[k]);
                }
                if (a != 0 && a != 2 && a != 4) continue;
                if (s == 1) continue;
                if (a == 2 || a == 0) goto del;
                if (BIN_VALUE(tmp, i+dx[0], j+dy[0]) == 0 || BIN_VALUE(tmp, i+dx[2], j+dy[2]) == 0 || (BIN_VALUE(tmp, i+dx[4], j+dy[4]) == 0 && BIN_VALUE(tmp, i+dx[6], j+dy[6]) == 0)) {
                    if ((BIN_VALUE(tmp, i+dx[0], j+dy[0]) + BIN_VALUE(tmp, i+dx[6], j+dy[6]) == 2) &&
                        (BIN_VALUE(tmp, i+dx[1], j+dy[1]) + BIN_VALUE(tmp, i+dx[5], j+dy[5]) >= 1) &&
                        (BIN_VALUE(tmp, i+dx[2], j+dy[2]) + BIN_VALUE(tmp, i+dx[3], j+dy[3]) + BIN_VALUE(tmp, i+dx[4], j+dy[4]) + BIN_VALUE(tmp, i+dx[7], j+dy[7]) == 0))
                        goto del;
                    else if ((BIN_VALUE(tmp, i+dx[0], j+dy[0]) + BIN_VALUE(tmp, i+dx[2], j+dy[2]) == 2) &&
                             (BIN_VALUE(tmp, i+dx[3], j+dy[3]) + BIN_VALUE(tmp, i+dx[7], j+dy[7]) >= 1) &&
                             (BIN_VALUE(tmp, i+dx[1], j+dy[1]) + BIN_VALUE(tmp, i+dx[4], j+dy[4]) + BIN_VALUE(tmp, i+dx[5], j+dy[5]) + BIN_VALUE(tmp, i+dx[6], j+dy[6]) == 0))
                        goto del;
                } else if (BIN_VALUE(tmp, i+dx[4], j+dy[4]) == 0 || BIN_VALUE(tmp, i+dx[6], j+dy[6]) == 0 || (BIN_VALUE(tmp, i+dx[0], j+dy[0]) == 0 && BIN_VALUE(tmp, i+dx[2], j+dy[2]) == 0)) {
                    if ((BIN_VALUE(tmp, i+dx[4], j+dy[4]) + BIN_VALUE(tmp, i+dx[2], j+dy[2]) == 2) &&
                        (BIN_VALUE(tmp, i+dx[1], j+dy[1]) + BIN_VALUE(tmp, i+dx[5], j+dy[5]) >= 1) &&
                        (BIN_VALUE(tmp, i+dx[0], j+dy[0]) + BIN_VALUE(tmp, i+dx[3], j+dy[3]) + BIN_VALUE(tmp, i+dx[6], j+dy[6]) + BIN_VALUE(tmp, i+dx[7], j+dy[7]) == 0))
                        goto del;
                    else if ((BIN_VALUE(tmp, i+dx[6], j+dy[6]) + BIN_VALUE(tmp, i+dx[4], j+dy[4]) == 2) &&
                             (BIN_VALUE(tmp, i+dx[3], j+dy[3]) + BIN_VALUE(tmp, i+dx[7], j+dy[7]) >= 1) &&
                             (BIN_VALUE(tmp, i+dx[1], j+dy[1]) + BIN_VALUE(tmp, i+dx[0], j+dy[0]) + BIN_VALUE(tmp, i+dx[5], j+dy[5]) + BIN_VALUE(tmp, i+dx[2], j+dy[2]) == 0))
                        goto del;
                }
                continue;
            del:
                change = true;
                tmp.setPixel(i, j, 0);
            }
        if (!change) break;
    }
    *dstImage = tmp.copy(0, 0, w, h);
}

void ImageEditor::neighborAve(int value) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            int sum = 0, tot = 0;
            for (int dx = -value; dx <= value; dx ++)
                for (int dy = -value; dy <= value; dy ++)
                    if (0 <= i+dx && i+dx < w && 0 <= j+dy && j+dy < w) {
                        sum += qGray(srcImage->pixel(i+dx, j+dy));
                        tot ++;
                    }
            dstImage->setPixel(i, j, sum / tot);
        }
}

void ImageEditor::neighborMed(int value) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) {
            vector<int> tmp;
            for (int dx = -value; dx <= value; dx ++)
                for (int dy = -value; dy <= value; dy ++)
                    if (0 <= i+dx && i+dx < w && 0 <= j+dy && j+dy < w)
                        tmp.push_back(qGray(srcImage->pixel(i+dx, j+dy)));
            __sort(tmp, 0, tmp.size() - 1);
            dstImage->setPixel(i, j, tmp[tmp.size() / 2]);
        }
}

void ImageEditor::neighborGaussian(int r, double s) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    double ker[2*r+1][2*r+1], tot = 0;
    for (int i = 0; i <= 2*r; i ++)
        for (int j = 0; j <= 2*r; j ++) {
            ker[i][j] = exp(-(SQR(i-r)+SQR(j-r)) * 1.0 / (2*SQR(s)));
            tot += ker[i][j];
        }
    for (int i = r; i < w - r; i ++)
        for (int j = r; j < h - r; j ++) {
            double sum = 0;
            for (int dx = -r; dx <= r; dx ++)
                for (int dy = -r; dy <= r; dy ++)
                    sum += qGray(srcImage->pixel(i+dx, j+dy)) * ker[dx+r][dy+r];
            sum /= tot;
            dstImage->setPixel(i, j, (int)sum);
        }
}

void ImageEditor::sobel() {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    int ker1[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
    int ker2[3][3] = {{-1,-2,-1}, {0,0,0}, {1,2,1}};
    for (int i = 1; i < w-1; i ++)
        for (int j = 1; j < h-1; j ++) {
            int sum1 = 0, sum2 = 0;
            for (int dx = -1; dx <= 1; dx ++)
                for (int dy = -1; dy <= 1; dy ++) {
                    sum1 += qGray(srcImage->pixel(i+dx, j+dy)) * ker1[dx+1][dy+1];
                    sum2 += qGray(srcImage->pixel(i+dx, j+dy)) * ker2[dx+1][dy+1];
                }
            dstImage->setPixel(i, j, min(255, abs(sum1)+abs(sum2)));
        }
}

void ImageEditor::roberts() {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);
    int ker1[2][2] = {{0,1}, {-1,0}};
    int ker2[2][2] = {{1,0}, {0,-1}};
    for (int i = 0; i < w-1; i ++)
        for (int j = 0; j < h-1; j ++) {
            int sum1 = 0, sum2 = 0;
            for (int dx = 0; dx <= 1; dx ++)
                for (int dy = 0; dy <= 1; dy ++) {
                    sum1 += qGray(srcImage->pixel(i+dx, j+dy)) * ker1[dx][dy];
                    sum2 += qGray(srcImage->pixel(i+dx, j+dy)) * ker2[dx][dy];
                }
            dstImage->setPixel(i, j, min(255, abs(sum1)+abs(sum2)));
        }
}

void ImageEditor::canny() {
    int w = srcImage->width(), h = srcImage->height();
    QImage* tmpImage = new QImage;
    *tmpImage = srcImage->copy(0, 0, w, h);
    *dstImage = srcImage->copy(0, 0, w, h);

    // Gaussian filter
    int r = 2;
    double s = 1.4;
    double ker[2*r+1][2*r+1], tot = 0;
    for (int i = 0; i <= 2*r; i ++)
        for (int j = 0; j <= 2*r; j ++) {
            ker[i][j] = exp(-(SQR(i-r)+SQR(j-r)) * 1.0 / (2*SQR(s)));
            tot += ker[i][j];
        }

    for (int i = r; i < w - r; i ++)
        for (int j = r; j < h - r; j ++) {
            double sum = 0;
            for (int dx = -r; dx <= r; dx ++)
                for (int dy = -r; dy <= r; dy ++)
                    sum += qGray(srcImage->pixel(i+dx, j+dy)) * ker[dx+r][dy+r];
            sum /= tot;
            tmpImage->setPixel(i, j, (int)sum);
        }

    // Sobel to find gradients
    int ker1[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
    int ker2[3][3] = {{1,2,1}, {0,0,0}, {-1,-2,-1}};

    int* gx = new int[w*h];
    int* gy = new int[w*h];
    double* theta = new double[w*h];
    memset(gx, 0, w*h*sizeof(int));
    memset(gy, 0, w*h*sizeof(int));

    int max_v = 0, min_v = 0x7fffffff;

    for (int j = 1; j < h-1; j ++)
        for (int i = 1; i < w-1; i ++) {
            int k = j*w+i;
            for (int dx = -1; dx <= 1; dx ++)
                for (int dy = -1; dy <= 1; dy ++) {
                    gx[k] += qGray(tmpImage->pixel(i+dx, j+dy)) * ker1[dy+1][dx+1];
                    gy[k] += qGray(tmpImage->pixel(i+dx, j+dy)) * ker2[dy+1][dx+1];
                }
            if (gx[k] == 0 && gy[k] == 0) theta[k] = 0;
            else theta[k] = atan2(gy[k], gx[k]) * 180 / 3.14159265;
            if (max_v < abs(gx[k]) + abs(gy[k])) max_v = abs(gx[k]) + abs(gy[k]);
            if (min_v > abs(gx[k]) + abs(gy[k])) min_v = abs(gx[k]) + abs(gy[k]);
        }

    double nf = (max_v - min_v) / 255.0;
    for (int j = 1; j < h-1; j ++)
        for (int i = 1; i < w-1; i ++) {
            int k = j*w+i;
            dstImage->setPixel(i, j, (int)((abs(gx[k])+abs(gy[k])-min_v)/nf+0.5));
        }

    *tmpImage = dstImage->copy(0, 0, w, h);

    // Non-maximum suppression
    for (int j = 1; j < h-1; j ++) {
        for (int i = 1; i < w-1; i ++) {
            int k = j*w+i;
            int t = qGray(tmpImage->pixel(i, j));
            if (fabs(theta[k]) < 22.5 || fabs(theta[k]) >= 157.5) {
                int n1 = qGray(tmpImage->pixel(i-1, j));
                int n2 = qGray(tmpImage->pixel(i+1, j));
                if (t <= n1 || t <= n2) dstImage->setPixel(i, j, 0);
            } else if ((22.5 <= theta[k] && theta[k] < 67.5) || (-157.5 <= theta[k] && theta[k] < -112.5)) {
                int n1 = qGray(tmpImage->pixel(i+1, j-1));
                int n2 = qGray(tmpImage->pixel(i-1, j+1));
                if (t <= n1 || t <= n2) dstImage->setPixel(i, j, 0);
            } else if (67.5 <= fabs(theta[k]) && fabs(theta[k]) < 112.5) {
                int n1 = qGray(tmpImage->pixel(i, j+1));
                int n2 = qGray(tmpImage->pixel(i, j-1));
                if (t <= n1 || t <= n2) dstImage->setPixel(i, j, 0);
            } else {
                int n1 = qGray(tmpImage->pixel(i-1, j-1));
                int n2 = qGray(tmpImage->pixel(i+1, j+1));
                if (t <= n1 || t <= n2) dstImage->setPixel(i, j, 0);
            }
        }
    }

    // Thresholding
    int* mark = new int[w*h];
    memset(mark, 0, w*h*sizeof(int));
    int th = 30, tl = 10;
    for (int j = 1; j < h-1; j ++)
        for (int i = 1; i < w-1; i ++) {
            int k = j*w+i;
            if (mark[k]) continue;
            int t = qGray(dstImage->pixel(i, j));
            if (t > th)
                __canny_threshold(i, j, mark, th, tl);
            else
                mark[k] = 1;
        }
    for (int j = 0; j < h; j ++)
        for (int i = 0; i < w; i ++) {
            int k = j*w+i;
            if (mark[k] != 2)
                dstImage->setPixel(i, j, 0);
            else if (mark[k] == 2)
                dstImage->setPixel(i, j, 255);
        }
}

void ImageEditor::inpainting(bool* markupRegion) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);

    // Setup parameters.
    int patchSize = 9;

    // Compute isophote.
    double* nx = new double[w*h];
    double* ny = new double[w*h];
    __get_gradient(srcImage, nx, ny);
    double* ix = new double[w*h];
    double* iy = new double[w*h];
    memcpy(ix, ny, w*h*sizeof(double));
    for (int i = 0; i < w; i ++)
        for (int j = 0; j < h; j ++) iy[i*h+j] = -ny[i*h+j];

    // Compute the number of pixels to be inpainted.
    // Init confidence terms.
    // Record the original image region.
    int remainPixels = 0;
    double* c = new double[w*h];
    double* d = new double[w*h];
    bool* sourceRegion = new bool[w*h];
    for (int x = 0; x < w; x ++)
        for (int y = 0; y < h; y ++) {
            int idx = x*h + y;
            remainPixels += (int)(markupRegion[idx]);
            c[idx] = 1-(int)(markupRegion[idx]);
            d[idx] = -0.1;
            sourceRegion[idx] = !markupRegion[idx];
        }

    // Start inpainting.
    while (remainPixels > 0) {

        // Find the border.
        vector<QPoint> borderPixels;
        for (int x = 0; x < w; x ++)
            for (int y = 0; y < h; y ++) {
                if (markupRegion[x*h+y]) continue;
                for (int dx = -1; dx <= 1; dx ++)
                    for (int dy = -1; dy <= 1; dy ++) {
                        if (dx == 0 && dy == 0) continue;
                        if (!isValidPixel(x+dx, y+dy, w, h)) continue;
                        if (markupRegion[(x+dx)*h+(y+dy)]) {
                            borderPixels.push_back(QPoint(x, y));
                            goto finish_checking_neighbor;
                        }

                    }
            finish_checking_neighbor:
                continue;
            }


        // Compute the normal direction of the border pixels and linear factors.
        // Compute the confidence of the border pixels.
        // Compute the priority of the border pixels.
        // Find the best border pixel.
        double maxPriority = -1e10;
        int px, py;
        __get_gradient(markupRegion, w, h, nx, ny);
        for (int i = 0; i < borderPixels.size(); i ++) {
            int x = borderPixels[i].x(), y = borderPixels[i].y();
            int idx = x*h + y;

            double n1 = nx[idx];
            double n2 = ny[idx];
            if (n1 != 0 || n2 != 0) {
                double n = sqrt(SQR(n1) + SQR(n2));
                n1 /= n;
                n2 /= n;
            }

            d[idx] = fabs(ix[idx]*n1 + iy[idx]*n2) + 1e-6;

            double patchSum = 0;
            int nrPatchPixels = 0;
            for (int dx = -patchSize/2; dx <= patchSize/2; dx ++)
                for (int dy = -patchSize/2; dy <= patchSize/2; dy ++) {
                    if (!isValidPixel(x+dx, y+dy, w, h)) continue;
                    nrPatchPixels ++;
                    patchSum += c[(x+dx)*h + (y+dy)];
                }
            c[idx] = patchSum / nrPatchPixels;

            double priority = c[idx] * d[idx];
            if (priority > maxPriority) {
                maxPriority = priority;
                px = x;
                py = y;
            }
        }

        // Find a best replacing patch from original image.
        int lx = min(patchSize/2, px);
        int rx = min(patchSize/2, w-px-1);
        int ly = min(patchSize/2, py);
        int ry = min(patchSize/2, h-py-1);

        double minDiff = 1e10;
        int qx, qy;

        for (int x = lx; x < w-rx; x ++)
            for (int y = ly; y < h-ry; y ++) {
                double diff = 0;
                for (int dx = -lx; dx <= rx; dx ++)
                    for (int dy = -ly; dy <= ry; dy ++) {

                        // Check if the pixels in source patch are all of original image.
                        int k1 = (x+dx)*h + (y+dy);
                        if (!sourceRegion[k1]) goto invalid_patch;

                        // Check if the pixel in border patch is marked up.
                        int k2 = (px+dx)*h + (py+dy);
                        if (markupRegion[k2]) continue;

                        // Compute the difference, using RGB-color space.
                        QColor cBorderPixel(dstImage->pixel(px+dx, py+dy));
                        QColor cSourcePixel(dstImage->pixel(x+dx, y+dy));
                        diff += colorDiff(cBorderPixel, cSourcePixel);
                        if (diff > minDiff) goto invalid_patch;
                    }

                minDiff = diff;
                qx = x;
                qy = y;

            invalid_patch:
                continue;
            }

        // Update the image, confidence, isophote and markupRegion.
        for (int dx = -lx; dx <= rx; dx ++)
            for (int dy = -ly; dy <= ry; dy ++) {
                QColor cReplace(srcImage->pixel(qx+dx, qy+dy));
                dstImage->setPixel(px+dx, py+dy, cReplace.rgb());

                int borderIdx = (px+dx)*h + (py+dy);
                int sourceIdx = (qx+dx)*h + (qy+dy);

                c[borderIdx] = c[px*h + py];

                ix[borderIdx] = ix[sourceIdx];
                iy[borderIdx] = iy[sourceIdx];

                remainPixels -= (int)(markupRegion[borderIdx]);
                markupRegion[borderIdx] = false;
            }
    }

    delete c;
    delete d;
    delete nx;
    delete ny;
    delete ix;
    delete iy;
    delete sourceRegion;
}

void ImageEditor::__sort(vector<int>& a, int l, int r) {
    int i = l, j = r, x = a[(l+r) >> 1];
    while (i <= j) {
        while (a[i] < x) i ++;
        while (a[j] > x) j --;
        if (i <= j) {
            int t = a[i];
            a[i] = a[j];
            a[j] = t;
            i ++;
            j --;
        }
    }
    if (l < j) __sort(a, l, j);
    if (i < r) __sort(a, i, r);
}

void ImageEditor::__canny_threshold(int x, int y, int *&mark, int th, int tl) {
    int k = y*dstImage->width() + x;
    if (mark[k]) return;

    int t = qGray(dstImage->pixel(x, y));
    if (t < tl) {
        mark[k] = 1;
        return;
    }

    mark[k] = 2;
    if (x > 0) __canny_threshold(x-1, y, mark, th, tl);
    if (x < dstImage->width()-1) __canny_threshold(x+1, y, mark, th, tl);
    if (y > 0) __canny_threshold(x, y-1, mark, th, tl);
    if (y < dstImage->height()-1) __canny_threshold(x, y+1, mark, th, tl);
}

void ImageEditor::__get_gradient(QImage *img, double *gx, double *gy) {
    int w = img->width(), h = img->height();

    for (int y = 0; y < h; y ++) gx[y] = qGray(img->pixel(0, y));
    for (int x = 1; x < w; x ++)
        for (int y = 0; y < h; y ++)
            gx[x*h + y] = qGray(img->pixel(x, y)) - qGray(img->pixel(x-1, y));

    for (int x = 0; x < w; x ++) gy[x*h] = qGray(img->pixel(x, 0));
    for (int x = 0; x < w; x ++)
        for (int y = 1; y < h; y ++)
            gy[x*h + y] = qGray(img->pixel(x, y)) - qGray(img->pixel(x, y-1));
}

void ImageEditor::__get_gradient(bool *binImg, int w, int h, double *gx, double *gy) {
    for (int y = 0; y < h; y ++) gx[y] = (int)(binImg[y]);
    for (int x = 1; x < w; x ++)
        for (int y = 0; y < h; y ++)
            gx[x*h + y] = (int)(binImg[x*h + y]) - (int)(binImg[(x-1)*h + y]);

    for (int x = 0; x < w; x ++) gy[x*h] = (int)(binImg[x*h]);
    for (int x = 0; x < w; x ++)
        for (int y = 1; y < h; y ++)
            gy[x*h + y] = (int)(binImg[x*h + y]) - (int)(binImg[x*h + (y-1)]);
}
