#include "nodesettingsdialog.h"
#include "ui_nodesettingsdialog.h"
#include <QCheckBox>
#include "node.h"
#include "processorfactory.h"

const QString NodeSettingsDialog::selects[5] = {"视频", "音频", "数据", "字幕", "附件"};

NodeSettingsDialog::NodeSettingsDialog(Node *node_, QWidget *parent)
    : QWidget(parent), node(node_)
    , ui(new Ui::NodeSettingsDialog) {
    ui->setupUi(this);

    ui->Name->setText(node->name);
    ui->Processer->setCurrentText(QString::fromStdString(node->processor->name()));
    for(int i = 0; i < nb_selects; ++i) {
        in[i] = new QCheckBox();
        in[i]->setText(selects[i]);
        in[i]->setChecked(node->inputs[i].enabled);
        ui->Inputs->addWidget(in[i]);
    }
    for(int i = 0; i < nb_selects; ++i) {
        out[i] = new QCheckBox();
        out[i]->setText(selects[i]);
        out[i]->setChecked(node->outputs[i].enabled);
        ui->Outputs->addWidget(out[i]);
    }
}

NodeSettingsDialog::~NodeSettingsDialog() {
    delete ui;
}

void NodeSettingsDialog::on_apply_clicked() {
    node->name = ui->Name->text();
    node->processor = ProcessorFactory::create(ui->Processer->currentText());
    for(int i = 0; i < nb_selects; ++i) {
        node->inputs[i].enabled = in[i]->isChecked();
    }
    for(int i = 0; i < nb_selects; ++i) {
        node->outputs[i].enabled = out[i]->isChecked();
    }
    node->updatePorts();
    node->update();
}

