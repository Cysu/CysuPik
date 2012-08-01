#ifndef GRAPH_H
#define GRAPH_H

#include <QPoint>

struct GraphNode {
    int id;
    QPoint loc;
    GraphNode* next;

    void addAdj(GraphNode& node);
};

#endif // GRAPH_H
