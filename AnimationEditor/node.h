#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include "AVFormat.h"
#include "settable.h"
#include "processor.h"
#include "nodeport.h"

class EditScene;
class Node : public QGraphicsObject, public Settable {
    Q_OBJECT
public:
    Node(QString name = "");
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent* ev)override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *ev)override;
    QWidget* newSettingMenu() override;
private:
    QString name;
    Processor* processor = new Processor();

    struct CheckablePort {
        bool enabled = true;
        NodePort* port = new NodePort();
    };
    using PortArray = std::array<CheckablePort, AVMEDIA_TYPE_NB>;
    PortArray inputs, outputs;
    int nb_inputs, nb_outputs;

    static void updateSidePorts(PortArray&, int xPos);
    static int enabledCount(PortArray&);
private slots:
    void updatePorts();
    friend class NodeSettingsDialog;
};

#endif // NODE_H
