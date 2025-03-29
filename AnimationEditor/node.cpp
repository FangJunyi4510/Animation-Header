#include "node.h"
#include "nodesettingsdialog.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

const int width = 100;
const int interval = 15;
const int padding = 10;

Node::Node(QString name_): name(name_) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    for(int i = 0; i < AVMEDIA_TYPE_NB; ++i) {
        inputs[i].port->setParentItem(this);
        outputs[i].port->setParentItem(this);
    }
    updatePorts();
}


void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // 绘制节点背景
    if(isSelected()) {
        painter->setBrush(QColor(200, 120, 60));
    } else {
        painter->setBrush(QColor(60, 60, 60));
    }
    painter->drawRoundedRect(boundingRect(), 5, 5);

    // 绘制标题
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-45, -20, 90, 40), Qt::AlignCenter, name);
}

QRectF Node::boundingRect() const {
    int height = qMax(2 * padding, (qMax(nb_inputs, nb_outputs) - 1) * interval + 2 * padding);
    return QRectF(-width / 2, -height / 2, width, height);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *ev) {
    QGraphicsObject::mousePressEvent(ev);

    qreal maxZ = 0.0;
    foreach(QGraphicsItem *item, scene()->items()) {
        if(item->zValue() > maxZ) {
            maxZ = item->zValue();
        }
    }
    this->setZValue(maxZ + 1.0);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *ev) {
    QGraphicsObject::mouseDoubleClickEvent(ev);
    menu()->show();
}

QWidget *Node::newSettingMenu() {
    NodeSettingsDialog *dialog = new NodeSettingsDialog(this);
    // 初始化当前值
    // dialog->setNodeName(name);
    // dialog->setInputMediaTypes(inputs);
    // dialog->setOutputMediaTypes(outputs);
    // dialog->setProcessorType(m_processor);
    return dialog;
}

int Node::enabledCount(PortArray & ports) {
    return std::count_if(ports.begin(), ports.end(), [](const CheckablePort & x) {
        return x.enabled;
    });
}

void Node::updateSidePorts(PortArray& ports, int xPos) {
    int count = enabledCount(ports);
    int idx = 0;
    for(auto& each : ports) {
        each.port->setVisible(each.enabled);
        if(!each.enabled) {
            continue;
        }
        each.port->setPos(xPos, idx * interval - (count - 1) * interval / 2);
        ++idx;
    }
}

void Node::updatePorts() {
    nb_inputs = enabledCount(inputs);
    nb_outputs = enabledCount(outputs);

    updateSidePorts(inputs, -width / 2);
    updateSidePorts(outputs, width / 2);
}
