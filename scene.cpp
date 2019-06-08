#include "scene.h"
#include <QDebug>

Scene::Scene(const std::vector<Figure*> &figures, const std::vector<Light> &lights, QWidget *parent)
    : QWidget(parent), figures(figures), lights(lights), fov(FOV) {
    setFixedSize(WIDTH, HEIGHT);
    image = new QImage(width(), height(), QImage::Format_RGB32);
}

const QImage &Scene::getImage() {
    return *image;
}

void Scene::renderMode() {
    rendering = true;
    update();
}

void Scene::viewMode() {
    rendering = false;
    update();
}

void Scene::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    createImage();
    painter.drawImage(0, 0, *image);
    if (!rendering) {
        painter.setPen(Qt::white);
        auto font = painter.font();
        font.setPointSize(static_cast<int>(font.pointSize() * 1.5));
        painter.setFont(font);
        painter.drawText(0, 0, width(), height(), Qt::AlignVCenter | Qt::AlignHCenter, "Click Render button!");
    }
}

Point3D Scene::reflect(const Point3D &I, const Point3D &N, double size) {
    return I - N * 2. * (I * N) * size;
}

Point3D Scene::refract(const Point3D &I, const Point3D &N, const double etaT, const double etaI) {
    auto cosi = - std::max(-1., std::min(1., I * N));
    if (cosi < 0) return refract(I, -N, etaI, etaT);
    auto eta = etaI / etaT;
    auto k = 1 - eta * eta * (1 - cosi * cosi);

    return k < 0 ? Point3D({1, 0, 0}) : I * eta + N * (eta * cosi - std::sqrt(k));
}

bool Scene::sceneIntersect(const Point3D &orig, const Point3D &dir, Point3D &hit, Point3D &N, Material &material) {
    auto dist = std::numeric_limits<double>::max();
    for (auto figure: figures) {
        double distI;
        if (figure->rayIntersect(orig, dir, distI) && distI < dist) {
            dist = distI;
            hit = orig + dir * distI;
            N = figure->normal(hit);
            material = figure->material;
        }
    }
    if (!rendering) material = Materials::wire;
    return dist < 1000;
}

Point3D Scene::castRay(const Point3D &orig, const Point3D &dir, size_t depth) {
    Point3D point, N;
    Material material;
    if (depth > DEPTH || !sceneIntersect(orig, dir, point, N, material) || (!rendering && depth > WF_MODE_DEPTH)) {
        return BACKGROUND;
    }

    auto diffuseLightIntensity = 0., specularLightIntensity = 0.;
    for (auto light: lights) {
        auto lightDir      = (light.position - point).normalize();
        auto lightDistance = (light.position - point).norm();

        if (rendering) {
            auto shadowOrig = lightDir * N < 0 ? point - N * OFFSET : point + N * OFFSET;
            Point3D shadowPt, shadowN;
            Material tmpmaterial;
            if (sceneIntersect(shadowOrig, lightDir, shadowPt, shadowN, tmpmaterial)
                    && (shadowPt - shadowOrig).norm() < lightDistance) continue;
        }

        diffuseLightIntensity  += light.intensity * std::max(0., lightDir * N);
        specularLightIntensity += std::pow(std::max(0., -reflect(-lightDir, N) * dir), material.specularExponent) * light.intensity;
    }
    if (!rendering) {
        return material.diffuseColor * diffuseLightIntensity * material.albedo[0];
    }

    auto reflectDirMain = reflect(dir, N).normalize();
    auto reflectOrig = reflectDirMain * N < 0 ? point - N * OFFSET : point + N * OFFSET;
    auto reflectColor = castRay(reflectOrig, reflectDirMain, depth + 1);

    auto refractDir = refract(dir, N, material.refractiveIndex).normalize();
    auto refractOrig = refractDir * N < 0 ? point - N * OFFSET : point + N * OFFSET;
    auto refractColor = castRay(refractOrig, refractDir, depth + 1);

    auto rez = material.diffuseColor * diffuseLightIntensity * material.albedo[0]
            + Point3D({1., 1., 1.}) * specularLightIntensity * material.albedo[1]
            + reflectColor * material.albedo[2]
            + refractColor * material.albedo[3];

    return rez;
}

void Scene::createImage() {
    auto percentage = 0;
    for (auto j = 0; j < height(); j++) {
        for (auto i = 0; i < width(); i++) {
            auto dirX =  (i + 0.5) -  width() / 2.;
            auto dirY = -(j + 0.5) + height() / 2.;
            auto dirZ = -height() / (2. * std::tan(fov / 2.));

            auto c = castRay(Point3D({0., 2., 0.}), Point3D({dirX, dirY, dirZ}).normalize());
            int colorData[3];
            for (uint k = 0; k < 3; k++) {
                colorData[k] = static_cast<int>(255 * std::max(0., std::min(1., c[k])));
            }
            image->setPixel(i, j, QColor(colorData[0], colorData[1], colorData[2]).rgba());
        }
        auto currentPercentage = static_cast<int>((j * 1. / height()) * 100);
        if (currentPercentage != percentage) {
            percentage = currentPercentage;
            qDebug() << percentage << "%";
        }
    }
}
