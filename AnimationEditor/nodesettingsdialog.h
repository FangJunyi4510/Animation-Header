#ifndef NODESETTINGSDIALOG_H
#define NODESETTINGSDIALOG_H

#include <QWidget>

namespace Ui {
class NodeSettingsDialog;
}

class Node;
class QCheckBox;
class NodeSettingsDialog : public QWidget {
    Q_OBJECT

public:
    explicit NodeSettingsDialog(Node* node, QWidget *parent = nullptr);
    ~NodeSettingsDialog();

private slots:
    void on_apply_clicked();

private:
    static const QString selects[5];
    static const int nb_selects = sizeof(selects) / sizeof(QString);

    Ui::NodeSettingsDialog *ui;
    Node* node;

    QCheckBox* in[nb_selects], *out[nb_selects];
};

#endif // NODESETTINGSDIALOG_H
