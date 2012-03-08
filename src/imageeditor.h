#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QImage>
#include <iostream>

using namespace std;

class ImageEditor {
public:
    ImageEditor();
    vector<int> getHistogram();
    void histogramEqualization();
    void setImage(QImage* srcImage, QImage* dstImage);

private:
    QImage* srcImage;
    QImage* dstImage;
};

#endif // IMAGEEDITOR_H
