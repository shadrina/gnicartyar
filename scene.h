#ifndef SCENE_H
#define SCENE_H

#include <QWidget>
#include <QImage>
#include <QPainter>

#include "utils.h"
#include "defaultvalues.h"

class Scene : public QWidget {
    Q_OBJECT

private:
    QImage *image;
    std::vector<Figure*> figures;
    std::vector<Light> lights;
    double fov;
    bool rendering = false;

public:
    Scene(const std::vector<Figure*> &figures, const std::vector<Light> &lights, QWidget *parent = nullptr);
    const QImage & getImage();
    void renderMode();
    void viewMode();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Point3D reflect(const Point3D &I, const Point3D &N, double size = 1.);
    Point3D refract(const Point3D &I, const Point3D &N, const double etaT, const double etaI = 1.);
    bool sceneIntersect(const Point3D &orig, const Point3D &dir, Point3D &hit, Point3D &N, Material &material);
    Point3D castRay(const Point3D &orig, const Point3D &dir, size_t depth = 0);
    void createImage();
};

#endif // SCENE_H
