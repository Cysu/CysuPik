#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QAction>
#include <QScrollBar>
#include <QScrollArea>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <vector>
#include "imageeditor.h"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void open();
    void processHistogramEqualization();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:

    void createActions();
    void createMenus();

    ImageEditor imageEditor;
    vector<QImage> images;

    QLabel* imageLabel;
    QScrollArea* scrollArea;

    QAction* openAct;
    QAction* histogramEqualizationAct;

    QMenu* fileMenu;
    QMenu* processMenu;
    QMenu* pointOperationMenu;

};

#endif // MAINWINDOW_H
