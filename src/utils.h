#ifndef UTILS_H
#define UTILS_H

#include <QThread>
#include <QTimer>

#define CREATE_ACTION(act, text, shortcut, func) \
    act = new QAction(tr(text), this); \
    if (shortcut != "") act->setShortcut(tr(shortcut)); \
    connect(act, SIGNAL(triggered()), this, SLOT(func()));

#define DO_ACTION(act, func) \
    recordAct(act); \
    imageEditor.func; \
    afterAct(act);

#define SQR(a) ((a) * (a))

#define BIN_VALUE(img, i, j) (qGray(img.pixel(i,j))/255)


#endif // UTILS_H
