#ifndef EDITSCENE_H
#define EDITSCENE_H

#include <QGraphicsScene>
#include "node.h"

class EditScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit EditScene(QObject *parent = nullptr);
    ~EditScene();
    void addNode();
signals:
    void info(const QString&);
private slots:
    void onSelectionChanged();
};

#endif // EDITSCENE_H
