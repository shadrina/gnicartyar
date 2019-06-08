#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    setWindowTitle("Raytracing");

    std::vector<Light> lights;
    lights.emplace_back(Light(Point3D({-20, 20, 20}), 1.5));
    lights.emplace_back(Light(Point3D({30, 50, -25}), 1.8));
    lights.emplace_back(Light(Point3D({-10, 30, -35}), 1.3));

    std::vector<Figure*> figures;
    figures.emplace_back(new Sphere(Point3D({-5, 0, -16}),          3, Materials::ivory));
    figures.emplace_back(new Sphere(Point3D({-1.0, -1.5, -12}),     2, Materials::glass));
    figures.emplace_back(new Sphere(Point3D({1.5, -0.5, -18}),      3, Materials::custom));
    figures.emplace_back(new Sphere(Point3D({4, 5.7, -18}),       3.5, Materials::redRubber));
    figures.emplace_back(new Sphere(Point3D({6, -2, -14}),        1.5, Materials::ivory));
    figures.emplace_back(new Sphere(Point3D({6, -2, -11}),         .5, Materials::custom));
    figures.emplace_back(new Plane(Point4D({0, 1, 0, 4}),   Materials::redRubber));
    figures.emplace_back(new Plane(Point4D({1, 0, 0, 10}),  Materials::sceneWalls));
    figures.emplace_back(new Plane(Point4D({0, 0, 1, 30}),  Materials::sceneWalls));
    figures.emplace_back(new Plane(Point4D({-1, 0, 0, 10}), Materials::sceneWalls));

    scene = new Scene(figures, lights, this);
    ui->sceneLayout->addWidget(scene);
}

MainWindow::~MainWindow() {
    delete ui;
    delete scene;
}

void MainWindow::on_actionSave_image_triggered() {
    auto image = scene->getImage();
    if (!image.save("./out.png")) {
        QMessageBox::critical(this, "Error", "Something went wrong while saving...");
    } else {
        ui->statusBar->showMessage("Saved as out.png");
    }
}

void MainWindow::on_actionRender_triggered() {
    ui->statusBar->showMessage("Start rendering...");
    scene->renderMode();
    ui->statusBar->showMessage("Rendering done!");
}

void MainWindow::on_actionSelect_view_triggered() {
    scene->viewMode();
}
