#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->Graph->setScene(&scene);
    ui->statusbar->addWidget(&info);
    connect(&scene, &EditScene::info, this, &MainWindow::on_info_change);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addUnit_triggered() {
    scene.addNode();
}


void MainWindow::on_exit_triggered() {
    close();
}

void MainWindow::on_info_change(const QString& text) {
    info.setText(text);
}

