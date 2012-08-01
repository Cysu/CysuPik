#include "inpaintingthread.h"

InpaintingThread::InpaintingThread(QObject *parent) :
    QThread(parent)
{
}

InpaintingThread::InpaintingThread(QImage *srcImage, QImage *dstImage, bool *markupRegion, bool *structRegion, vector<vector<QPoint> > *lines, QObject* parent) : QThread(parent) {
    this->srcImage = srcImage;
    this->dstImage = dstImage;
    this->markupRegion = markupRegion;
    this->structRegion = structRegion;
    this->lines = lines;
}

void InpaintingThread::run() {
    this->inpainting(markupRegion, structRegion, *lines);
}

void InpaintingThread::inpainting(bool* markupRegion, bool* structRegion, vector< vector<QPoint> >& lines) {
    int w = srcImage->width(), h = srcImage->height();
    *dstImage = srcImage->copy(0, 0, w, h);

    for (int x = 0; x < w; x ++)
        for (int y = 0; y < h; y ++)
            if (markupRegion[x*h + y])
                dstImage->setPixel(x, y, QColor(Qt::blue).rgb());

    // Setup some parameters.
    int structPatchSize = 5;
    int step = 2;
    int ks = 50, ki = 2;

    // Structure propagation.
    // Build up the structure graph to be filled up and initialize the source structure pixels.
    vector<GraphNode> dstGraph;
    vector<QPoint> srcPixels;
    bool isSingle[lines.size()];
    __inpainting_build_graph(markupRegion, structRegion, lines, step, structPatchSize, dstGraph, srcPixels, isSingle);

    // Dynamic programming.
    bool* mark = new bool[w*h];
    int srcStructRange = 1;

    for (int lineIdx = 0; lineIdx < lines.size(); lineIdx ++) {
        if (!isSingle[lineIdx]) continue;
        vector<QPoint>& line = lines[lineIdx];

        memset(mark, false, w*h*sizeof(bool));
        vector<QPoint> srcStructPixels, dstStructPixels;
        int cont = 0;

        for (int j = 0; j < line.size(); j ++) {
            int x = line[j].x(), y = line[j].y();
            if (!markupRegion[x*h + y]) {
                for (int dx = -srcStructRange; dx <= srcStructRange; dx ++)
                    for (int dy = -srcStructRange; dy <= srcStructRange; dy ++) {
                        if (!__inpainting_valid_src_struct(markupRegion, x+dx, y+dy, w, h, structPatchSize)) continue;
                        if (mark[(x+dx)*h + (y+dy)]) continue;
                        mark[(x+dx)*h + (y+dy)] = true;
                        srcStructPixels.push_back(QPoint(x+dx, y+dy));
                    }
            } else {
                cont ++;
                if (cont % step != 1) continue;
                dstStructPixels.push_back(line[j]);
            }
        }

        int srcSize = srcStructPixels.size(), dstSize = dstStructPixels.size();
        double* f = new double[srcSize*dstSize];
        int* prev = new int[srcSize*dstSize];

        for (int j = 0; j < srcSize; j ++) {
            f[j] = ks * __inpainting_struct_diff(structRegion, dstStructPixels[0], srcStructPixels[j], structPatchSize) +
                   ki * __inpainting_boundary_diff(markupRegion, dstStructPixels[0], srcStructPixels[j], structPatchSize);
            prev[j] = -1;
        }

        for (int i = 1; i < dstSize; i ++)
            for (int j = 0; j < srcSize; j ++) {
                int idx1 = i*srcSize + j;
                f[idx1] = DOUBLE_INFI;
                for (int k = 0; k < srcSize; k ++) {
                    int idx2 = (i-1)*srcSize + k;
                    double coh = __inpainting_coherence(dstStructPixels[i-1], dstStructPixels[i],
                                                        srcStructPixels[k], srcStructPixels[j],
                                                        structPatchSize);
                    if (f[idx1] > f[idx2] + coh) {
                        f[idx1] = f[idx2] + coh;
                        prev[idx1] = idx2;
                    }
                }

                f[idx1] += ks * __inpainting_struct_diff(structRegion, dstStructPixels[i], srcStructPixels[j], structPatchSize) +
                           ki * __inpainting_boundary_diff(markupRegion, dstStructPixels[i], srcStructPixels[j], structPatchSize);
            }

        double minf = DOUBLE_INFI;
        int minIdx = -1;
        for (int j = 0; j < srcSize; j ++) {
            int idx = (dstSize-1)*srcSize + j;
            if (minf > f[idx]) {
                minf = f[idx];
                minIdx = idx;
            }
        }

        int i = dstSize-1;
        while (minIdx != -1) {
            int j = minIdx % srcSize;
            for (int dx = -structPatchSize/2; dx <= structPatchSize/2; dx ++)
                for (int dy = -structPatchSize/2; dy <= structPatchSize/2; dy ++) {
                    int x1 = dstStructPixels[i].x()+dx, y1 = dstStructPixels[i].y()+dy;
                    int x2 = srcStructPixels[j].x()+dx, y2 = srcStructPixels[j].y()+dy;
                    dstImage->setPixel(x1, y1, srcImage->pixel(x2, y2));
                    markupRegion[x1*h + y1] = false;
                }
            minIdx = prev[minIdx];
            i --;
        }

        delete f;
        delete prev;
    }
    delete mark;

    emit partDone();

    // Belief propagation.
    int n = dstGraph.size(), m = srcPixels.size();
    double* M = new double[n*n*m];
    double* bufM = new double[n*n*m];
    memset(M, 0, n*n*m*sizeof(double));

    bool* isBoundary = new bool[w*h*sizeof(bool)];
    memset(isBoundary, false, w*h*sizeof(bool));
    for (int i = 0; i < n; i ++) {
        int x = dstGraph[i].loc.x(), y = dstGraph[i].loc.y();
        for (int dx = -structPatchSize/2; dx <= structPatchSize/2; dx ++)
            for (int dy = -structPatchSize/2; dy <= structPatchSize/2; dy ++) {
                int x1 = x+dx, y1 = y+dy;
                if (!markupRegion[x1*h + y1]) {
                    isBoundary[x*h + y] = true;
                    goto label_isboundary;
                }
            }
    label_isboundary:
        continue;
    }

    double* minValue = new double[m];

    while (true) {
        bool update = false;
        memcpy(bufM, M, n*n*m*sizeof(double));
        for (int i = 0; i < n; i ++) {
            // Find the neighbours of node i.
            GraphNode& node = dstGraph[i];
            GraphNode* p = node.next;
            vector<int> nbsIdx;
            while (p) {
                nbsIdx.push_back(p->id);
                p = p->next;
            }

            for (int nbj = 0; nbj < nbsIdx.size(); nbj ++) {
                int j = nbsIdx[nbj];
                for (int xj = 0; xj < m; xj ++) minValue[xj] = DOUBLE_INFI;
                for (int xi = 0; xi < m; xi ++) {
                    double E1 = ks * __inpainting_struct_diff(structRegion, node.loc, srcPixels[xi], structPatchSize);
                    if (isBoundary[node.loc.x()*h + node.loc.y()])
                        E1 += ki * __inpainting_boundary_diff(markupRegion, node.loc, srcPixels[xi], structPatchSize);
                    double sum = 0;
                    for (int k = 0; k < nbsIdx.size(); k ++) {
                        if (k == nbj) continue;
                        sum += bufM[BPIDX(nbsIdx[k], i, xi)];
                    }
                    for (int xj = 0; xj < m; xj ++) {
                        double E2 = __inpainting_coherence(node.loc, dstGraph[j].loc, srcPixels[xi], srcPixels[xj], structPatchSize);
                        minValue[xj] = min(minValue[xj], E1+E2+sum);
                    }
                }
                for (int xj = 0; xj < m; xj ++) {
                    if (fabs(M[BPIDX(i, j, xj)] - minValue[xj]) > 1e-3) {
                        update = true;
                    }
                    M[BPIDX(i, j, xj)] = minValue[xj];
                }
            }
        }
        if (!update) break;
    }


    bool* bufMarkupRegion = new bool[w*h*sizeof(bool)];
    memcpy(bufMarkupRegion, markupRegion, w*h*sizeof(bool));

    // Get the best labels.
    for (int i = 0; i < n; i ++) {
        GraphNode& node = dstGraph[i];
        GraphNode* p = node.next;
        vector<int> nbsIdx;
        while (p) {
            nbsIdx.push_back(p->id);
            p = p->next;
        }

        double minValue = DOUBLE_INFI;
        int minArg;
        for (int xi = 0; xi < m; xi ++) {
            double E1 = ks * __inpainting_struct_diff(structRegion, dstGraph[i].loc, srcPixels[xi], structPatchSize);
            if (E1 > minValue) continue;
            if (isBoundary[node.loc.x()*h + node.loc.y()]) {
                E1 += ki * __inpainting_boundary_diff(bufMarkupRegion, dstGraph[i].loc, srcPixels[xi], structPatchSize);
                if (E1 > minValue) continue;
            }
            double sum = 0;
            for (int k = 0; k < nbsIdx.size(); k ++)
                sum += M[BPIDX(nbsIdx[k], i, xi)];
            if (minValue > sum + E1) {
                minValue = sum + E1;
                minArg = xi;
            }
        }

        // Inpainting i with xi.
        for (int dx = -structPatchSize/2; dx <= structPatchSize/2; dx ++)
            for (int dy = -structPatchSize/2; dy <= structPatchSize/2; dy ++) {
                int x1 = dstGraph[i].loc.x()+dx, y1 = dstGraph[i].loc.y()+dy;
                int x2 = srcPixels[minArg].x()+dx, y2 = srcPixels[minArg].y()+dy;
                dstImage->setPixel(x1, y1, srcImage->pixel(x2, y2));
                markupRegion[x1*h + y1] = false;
            }
    }

    delete bufMarkupRegion;
    delete isBoundary;
    delete minValue;

    emit partDone();

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
        double maxPriority = -1e20;
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

        double minDiff = 1e20;
        int qx, qy;

        for (int x = lx; x < w-rx; x ++)
            for (int y = ly; y < h-ry; y ++) {
                double diff = 0;
//                double alpha = 10-9*exp(-(SQR(x-px)+SQR(y-py))/400.0);
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
                int borderIdx = (px+dx)*h + (py+dy);
                int sourceIdx = (qx+dx)*h + (qy+dy);

                if (!markupRegion[borderIdx]) continue;

                QColor cReplace(srcImage->pixel(qx+dx, qy+dy));
                __color_random_adjust(&cReplace);
                dstImage->setPixel(px+dx, py+dy, cReplace.rgb());

                c[borderIdx] = c[px*h + py];

                ix[borderIdx] = ix[sourceIdx];
                iy[borderIdx] = iy[sourceIdx];

                remainPixels --;
                markupRegion[borderIdx] = false;
            }
        emit partDone();
    }

    delete c;
    delete d;
    delete nx;
    delete ny;
    delete ix;
    delete iy;
    delete sourceRegion;
}

void InpaintingThread::__inpainting_build_graph(bool *markupRegion, bool *structRegion, vector<vector<QPoint> > &lines,
                                           int step, int structPatchSize, vector<GraphNode>& dst, vector<QPoint>& src, bool* isSingle) {
    int w = srcImage->width(), h = srcImage->height();

    // Find the conjunction points.
    bool* mark = new bool[w*h];
    memset(mark, false, w*h*sizeof(bool));
    memset(isSingle, true, lines.size()*sizeof(bool));

    for (int i = 0; i < lines.size(); i ++) {
        vector<QPoint>& lineP = lines[i];
        for (int j = i + 1; j < lines.size(); j ++) {
            vector<QPoint>& lineQ = lines[j];
            int minDist = 0x7fffffff, minK;
            for (int k1 = 0; k1 < lineP.size(); k1 ++) {
                QPoint& p = lineP[k1];
                if (!markupRegion[p.x()*h + p.y()]) continue;
                for (int k2 = 0; k2 < lineQ.size(); k2 ++) {
                    QPoint& q = lineQ[k2];
                    if (!markupRegion[q.x()*h + q.y()]) continue;
                    if (minDist > (p-q).manhattanLength()) {
                        minDist = (p-q).manhattanLength();
                        minK = k1;
                    }
                }
            }
            QPoint& p = lineP[minK];
            if (minDist <= 1 && !mark[p.x()*h + p.y()]) {
                mark[p.x()*h + p.y()] = true;
                GraphNode gn = {dst.size(), p, NULL};
                dst.push_back(gn);
                isSingle[i] = false;
                isSingle[j] = false;
            }
        }
    }

    int nrConj = dst.size();

    // Build the graph, connect edges.
    for (int i = 0; i < lines.size(); i ++) {
        if (isSingle[i]) continue;

        vector<QPoint>& line = lines[i];
        bool first = true;
        for (int k = 0; k < line.size(); k += step) {
            QPoint& p = line[k];
            if (!markupRegion[p.x()*h + p.y()]) continue;
            if (mark[p.x()*h + p.y()]) continue;

            GraphNode gn = {dst.size(), p, NULL};
            dst.push_back(gn);

            if (!first) {
                QPoint& q = dst[dst.size()-2].loc;
                // Check if there is a conjunction point between p and q.
                bool flag = false;
                for (int j = 0; j < nrConj; j ++) {
                    QPoint& t = dst[j].loc;
                    if (min(p.x(), q.x()) <= t.x() && t.x() <= max(p.x(), q.x()) &&
                            min(p.y(), q.y()) <= t.y() && t.y() <= max(p.y(), q.y())) {
                        flag = true;
                        dst[dst.size()-1].addAdj(dst[j]);
                        dst[j].addAdj(dst[dst.size()-1]);
                        dst[j].addAdj(dst[dst.size()-2]);
                        dst[dst.size()-2].addAdj(dst[j]);
                        break;
                    }
                }
                if (!flag) {
                    dst[dst.size()-1].addAdj(dst[dst.size()-2]);
                    dst[dst.size()-2].addAdj(dst[dst.size()-1]);
                }
            } else {
                first = false;
            }
        }
    }

    // Find the source pixels.
    memset(mark, false, sizeof(mark));
    int srcStructRange = 1;
    for (int i = 0; i < lines.size(); i ++) {
        if (isSingle[i]) continue;
        vector<QPoint>& line = lines[i];
        for (int j = 0; j < line.size(); j ++) {
            int x = line[j].x(), y = line[j].y();
            if (markupRegion[x*h + y]) continue;
            for (int dx = -srcStructRange; dx <= srcStructRange; dx ++)
                for (int dy = -srcStructRange; dy <= srcStructRange; dy ++) {
                    if (!__inpainting_valid_src_struct(markupRegion, x+dx, y+dy, w, h, structPatchSize)) continue;
                    if (mark[(x+dx)*h + (y+dy)]) continue;
                    mark[(x+dx)*h + (y+dy)] = true;
                    src.push_back(QPoint(x+dx, y+dy));
                }
        }
    }
}

bool InpaintingThread::__inpainting_valid_src_struct(bool *region, int x, int y, int w, int h, int patchSize) {
    for (int dx = -patchSize/2; dx <= patchSize/2; dx ++)
        for (int dy = -patchSize/2; dy <= patchSize/2; dy ++) {
            if (!isValidPixel(x+dx, y+dy, w, h)) return false;
            if (region[(x+dx)*h + (y+dy)]) return false;
        }
    return true;
}

double InpaintingThread::__inpainting_struct_diff(bool *region, QPoint &dst, QPoint &src, int patchSize) {
    int w = srcImage->width(), h = srcImage->height();
    int xd = dst.x(), yd = dst.y();
    int xs = src.x(), ys = src.y();
    vector<QPoint> dstPixels, srcPixels;
    for (int dx = -patchSize/2; dx <= patchSize/2; dx ++)
        for (int dy = -patchSize/2; dy <= patchSize/2; dy ++) {
            int x = xd+dx, y = yd+dy;
            if (region[x*h + y])
                dstPixels.push_back(QPoint(x, y));
            x = xs+dx, y = ys+dy;
            if (region[x*h + y])
                srcPixels.push_back(QPoint(x, y));
        }

    double s1 = 0;
    for (int i = 0; i < srcPixels.size(); i ++) {
        int minDist = 0x7fffffff;
        int x1 = srcPixels[i].x()-xs, y1 = srcPixels[i].y()-ys;
        for (int j = 0; j < dstPixels.size(); j ++) {
            int x2 = dstPixels[j].x()-xd, y2 = dstPixels[j].y()-yd;
            int dist = SQR(x1-x2)+SQR(y1-y2);
            if (dist < minDist) minDist = dist;
        }
        s1 += sqrt(minDist);
    }

    double s2 = 0;
    for (int i = 0; i < dstPixels.size(); i ++) {
        int minDist = 0x7fffffff;
        int x1 = dstPixels[i].x()-xd, y1 = dstPixels[i].y()-yd;
        for (int j = 0; j < srcPixels.size(); j ++) {
            int x2 = srcPixels[j].x()-xs, y2 = srcPixels[j].y()-ys;
            int dist = SQR(x1-x2)+SQR(y1-y2);
            if (dist < minDist) minDist = dist;
        }
        s2 += sqrt(minDist);
    }

    return s1/srcPixels.size()+s2/dstPixels.size();
}

double InpaintingThread::__inpainting_boundary_diff(bool *region, QPoint &dst, QPoint &src, int patchSize) {
    int w = srcImage->width(), h = srcImage->height();
    int xd = dst.x(), yd = dst.y();
    int xs = src.x(), ys = src.y();

    double ret = 0;
    int area = 0;
    for (int dx = -patchSize/2; dx <= patchSize/2; dx ++)
        for (int dy = -patchSize/2; dy <= patchSize/2; dy ++) {
            int x1 = xd+dx, y1 = yd+dy;
            int x2 = xs+dx, y2 = ys+dy;
            area ++;
            if (region[x1*h + y1]) continue;
            ret += colorDiff(srcImage->pixel(x1, y1), srcImage->pixel(x2, y2));
        }
    return ret/area;
}

double InpaintingThread::__inpainting_coherence(QPoint &dst1, QPoint &dst2, QPoint &src1, QPoint &src2, int patchSize) {
    double ret = 0;
    int area = 0;
    for (int dx = -patchSize/2; dx <= patchSize/2; dx ++)
        for (int dy = -patchSize/2; dy <= patchSize/2; dy ++) {
            int x = dst1.x()+dx, y = dst1.y()+dy;
            if (abs(x-dst2.x()) > patchSize/2 || abs(y-dst2.y()) > patchSize/2) continue;
            ret += colorDiff(srcImage->pixel(src1.x()+dx, src1.y()+dy),
                             srcImage->pixel(src2.x()+x-dst2.x(), src2.y()+y-dst2.y()));
            area ++;
        }
    return ret/area;
}

void InpaintingThread::__color_random_adjust(QColor *color) {
    int sign = (rand() % 2 == 0) ? 1 : -1;
    int percent = rand() % 5;
    double ratio = 1+sign*0.01*percent;
    int r = (int)(color->red()*ratio + 0.5);
    int g = (int)(color->green()*ratio + 0.5);
    int b = (int)(color->blue()*ratio + 0.5);
    r = min(r, 255);
    g = min(g, 255);
    b = min(b, 255);
    r = max(r, 0);
    g = max(g, 0);
    b = max(b, 0);
    color->setRgb(r, g, b);
}

void InpaintingThread::__get_gradient(QImage *img, double *gx, double *gy) {
    int w = img->width(), h = img->height();

    for (int y = 0; y < h; y ++) gx[y] = qGray(img->pixel(0, y));
    for (int x = 1; x < w; x ++)
        for (int y = 0; y < h; y ++) {
            QColor c1(img->pixel(x, y));
            QColor c2(img->pixel(x-1, y));
            gx[x*h + y] = (c1.red()-c2.red() + c1.green()-c2.green() + c1.blue()-c2.blue())/(3.0*255);
        }

    for (int x = 0; x < w; x ++) gy[x*h] = qGray(img->pixel(x, 0));
    for (int x = 0; x < w; x ++)
        for (int y = 1; y < h; y ++) {
            QColor c1(img->pixel(x, y));
            QColor c2(img->pixel(x, y-1));
            gy[x*h + y] = (c1.red()-c2.red() + c1.green()-c2.green() + c1.blue()-c2.blue())/(3.0*255);
        }
}

void InpaintingThread::__get_gradient(bool *binImg, int w, int h, double *gx, double *gy) {
    for (int y = 0; y < h; y ++) gx[y] = (int)(binImg[y]);
    for (int x = 1; x < w; x ++)
        for (int y = 0; y < h; y ++)
            gx[x*h + y] = (int)(binImg[x*h + y]) - (int)(binImg[(x-1)*h + y]);

    for (int x = 0; x < w; x ++) gy[x*h] = (int)(binImg[x*h]);
    for (int x = 0; x < w; x ++)
        for (int y = 1; y < h; y ++)
            gy[x*h + y] = (int)(binImg[x*h + y]) - (int)(binImg[x*h + (y-1)]);
}
