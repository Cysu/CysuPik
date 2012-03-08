#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <QColor>
#include <iostream>

using namespace std;

class ImageEditor {
public:
    ImageEditor();

    static void getHistogram(QImage* image, int* histogram);

    void histogramEqualization();
    void setImage(QImage* srcImage, QImage* dstImage);

private:
    QImage* srcImage;
    QImage* dstImage;
};

#endif // IMAGEEDITOR_H
