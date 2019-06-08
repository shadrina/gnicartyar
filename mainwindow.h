#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "scene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSave_image_triggered();
    void on_actionRender_triggered();
    void on_actionSelect_view_triggered();

private:
    Ui::MainWindow *ui;
    Scene *scene;
};

#endif // MAINWINDOW_H
