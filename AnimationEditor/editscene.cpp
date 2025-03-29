#include "editscene.h"

EditScene::EditScene(QObject *parent)
    : QGraphicsScene{parent} {
    connect(this, &QGraphicsScene::selectionChanged, this, &EditScene::onSelectionChanged);
}

EditScene::~EditScene() {
    disconnect(this);
}

void EditScene::addNode() {
    auto newNode = new Node("1234");
    addItem(newNode);
}

void EditScene::onSelectionChanged() {
    auto items = selectedItems();
    for(const auto& each : items) {
        auto node = dynamic_cast<Node*>(each);
        if(!node) {
            continue;
        }
        emit info("Items count: " + QString::number(items.size()) + " the first's name: ");
    }
}
