#ifndef NODEPORT_H
#define NODEPORT_H

#include <QGraphicsObject>

class Node;
class NodePort : public QGraphicsObject {
    Q_OBJECT
public:
    NodePort();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
private:
};

#endif // NODEPORT_H
