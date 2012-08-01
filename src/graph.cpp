#include "graph.h"

void GraphNode::addAdj(GraphNode &node) {
    GraphNode* newNode = new GraphNode;
    newNode->id = node.id;
    newNode->loc = node.loc;
    newNode->next = this->next;
    this->next = newNode;
}
