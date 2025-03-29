#include "nodeport.h"
#include <QPainter>

NodePort::NodePort() {

}

void NodePort::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setPen(Qt::white);
    painter->drawRect(boundingRect());
}

const int NodePortSize = 4;
QRectF NodePort::boundingRect() const {
    return QRectF(-NodePortSize / 2, -NodePortSize / 2, NodePortSize, NodePortSize);
}
