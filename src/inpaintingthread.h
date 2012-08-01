#ifndef INPAINTINGTHREAD_H
#define INPAINTINGTHREAD_H

#include <QThread>
#include <QImage>
#include <QColor>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "graph.h"
#include "utils.h"
#include "const.h"

#define BPIDX(i, j, xj) (i*n*m + j*m + xj)

using namespace std;

class InpaintingThread : public QThread
{
    Q_OBJECT
public:

    inline static bool isValidPixel(int x, int y, int w, int h) {
        return (0 <= x && x < w && 0 <= y && y < h);
    }
    inline static double colorDiff(QColor c1, QColor c2) {
        return sqrt((SQR(c1.red()-c2.red()) + SQR(c1.green()-c2.green()) + SQR(c1.blue()-c2.blue()))/3);
    }

    explicit InpaintingThread(QObject *parent = 0);
    explicit InpaintingThread(QImage* srcImage, QImage* dstImage,
                              bool* markupRegion, bool* structRegion,
                              vector< vector<QPoint> >* lines,
                              QObject* parent = 0);
    void run();

    
signals:

    void partDone();
    
public slots:

private:

    QImage* srcImage;
    QImage* dstImage;
    bool* markupRegion;
    bool* structRegion;
    vector< vector<QPoint> >* lines;

    void inpainting(bool* markupRegion, bool* structRegion, vector< vector<QPoint> >& lines);
    void __inpainting_build_graph(bool* markupRegion, bool* structRegion, vector< vector<QPoint> >& lines,
                                  int step, int structPatchSize, vector<GraphNode>& dst, vector<QPoint>& src, bool* isSingle);
    double __inpainting_struct_diff(bool* region, QPoint& dst, QPoint& src, int patchSize);
    double __inpainting_boundary_diff(bool* region, QPoint& dst, QPoint& src, int patchSize);
    double __inpainting_coherence(QPoint& dst1, QPoint& dst2, QPoint& src1, QPoint& src2, int patchSize);
    bool __inpainting_valid_src_struct(bool* region, int x, int y, int w, int h, int patchSize);


    void __get_gradient(QImage* img, double* gx, double* gy);
    void __get_gradient(bool* binImg, int w, int h, double* gx, double* gy);
    void __color_random_adjust(QColor* color);

    
};

#endif // INPAINTINGTHREAD_H
