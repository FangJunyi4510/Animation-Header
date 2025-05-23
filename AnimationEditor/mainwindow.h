#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "editscene.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addUnit_triggered();
    void on_exit_triggered();
    void on_info_change(const QString& info);

private:
    Ui::MainWindow *ui;
    EditScene scene;
    QLabel info;
};
#endif // MAINWINDOW_H
