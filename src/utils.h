#ifndef UTILS_H
#define UTILS_H

#define CREATE_ACTION(act, text, shortcut, func) \
    act = new QAction(tr(text), this); \
    if (shortcut != "") act->setShortcut(tr(shortcut)); \
    connect(act, SIGNAL(triggered()), this, SLOT(func()));

#define DO_ACTION(act, func) \
    recordAct(act); \
    imageEditor.func; \
    afterAct(act);

#endif // UTILS_H
