#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "geometry.h"
#include "defaultvalues.h"

struct Light {
    Point3D position;
    double intensity;

    Light(const Point3D &p, const double i) : position(p), intensity(i) {}
};

struct Material {
    double refractiveIndex;
    Point4D albedo;
    Point3D diffuseColor;
    double specularExponent;

    Material(const double r, const Point4D &a, const Point3D &color, const double spec)
        : refractiveIndex(r), albedo(a), diffuseColor(color), specularExponent(spec) {}

    Material() : refractiveIndex(1), albedo({1, 0, 0, 0}), diffuseColor(), specularExponent() {}
};

namespace Materials {
    static const Material ivory(1.0, Point4D({.6, .3, .1, 0.}), Point3D({.4, .4, .3}), 50.);
    static const Material glass(1.5, Point4D({0.,  .5, .1, .8}), Point3D({.6, .7, .8}), 125.);
    static const Material mirror(1.0, Point4D({0., 10., .8, 0.}), Point3D({1., 1., 1.}), 1425.);
    static const Material redRubber(1.0, Point4D({.9, .1, 0., 0.}), Point3D({.3, .1, .1}), 10.);

    static const Material sceneWalls(1.0, Point4D({.8, .2, .2, 0.}), Point3D({.7, .7, .7}), 100.);
    static const Material custom(1.0, Point4D({.4, .3, .1, 0.}), Point3D({.6, .3, .6}), 50.);
    static const Material wire(1.0, Point4D({.6, .3, .1, 0.}), Point3D({.1, .1, .1}), 50.);
}

struct Figure {
    Material material;

    Figure(const Material &m) : material(m) {}
    virtual ~Figure();

    virtual bool rayIntersect(const Point3D &orig, const Point3D &dir, double &t0) const = 0;
    virtual Point3D normal(const Point3D &hit) const = 0;
};

struct Plane : public Figure {
    Point4D abcd;
    Point3D normalV;

    static Point<3> cross(Point<3> v1, Point<3> v2) {
        return Point<3>(
            {v1[1] * v2[2] - v1[2] * v2[1],
             v1[2] * v2[0] - v1[0] * v2[2],
             v1[0] * v2[1] - v1[1] * v2[0]});
    }

    Plane(const Point4D &abcd, const Material &m) : Figure(m), abcd(abcd), normalV({abcd[0], abcd[1], abcd[2]}) {
        normalV.normalize();
    }
    Plane(const Point3D &p1, const Point3D &p2, const Point3D &p3, const Material &m) : Figure(m) {
        auto v1 = p3 - p1;
        auto v2 = p2 - p1;

        auto cp = cross(v1, v2);
        abcd = Point4D({cp[0], cp[1], cp[2], cp * p3});
        normalV = Point3D({abcd[0], abcd[1], abcd[2]});
    }
    ~Plane() override;

    bool rayIntersect(const Point3D &orig, const Point3D &dir, double &t0) const override {
        auto dot = normalV * dir;
        if (dot > OFFSET) return false;
        auto tRez = -((normalV * orig) + abcd[3]) / dot;
        auto hit = orig + dir * tRez;
        if (tRez < OFFSET || !inBox(hit)) return false;
        t0 = tRez;
        return true;
    }

    Point3D normal(const Point3D &) const override {
        return normalV;
    }

    bool pointOnPlane(const Point3D &p) {
        return std::abs(abcd[0] * p[0] + abcd[1] * p[1] + abcd[2] * p[2] + abcd[3]) <= 0.5;
    }

private:
    bool inBox(const Point3D &p) const {
        return     p[2] <= ZN             && p[2] >= ZF
                && p[1] >= SCENE_Y_BOTTOM && p[1] <= SCENE_Y_BOTTOM + SCENE_HEIGHT
                && p[0] >= SCENE_X_LEFT   && p[0] <= SCENE_X_LEFT + SCENE_WIDTH;
    }

    bool valuesAreSimilar(double d1, double d2) const {
        return std::abs(d1 - d2) <= 0.1;
    }

    bool atBorders(const Point3D &p) const {
        auto c1 = valuesAreSimilar(p[2], ZN) || valuesAreSimilar(p[2], ZF) ? 1 : 0;
        auto c2 = valuesAreSimilar(p[1], SCENE_Y_BOTTOM) || valuesAreSimilar(p[1], SCENE_Y_BOTTOM + SCENE_HEIGHT) ? 1 : 0;
        auto c3 = valuesAreSimilar(p[0], SCENE_X_LEFT)   || valuesAreSimilar(p[0], SCENE_X_LEFT + SCENE_WIDTH) ? 1 : 0;
        return c1 + c2 + c3 >= 2;
    }
};

struct Sphere : public Figure {
    Point3D center;
    double radius;

    Sphere(const Point3D &c, const double r, const Material &m) : Figure(m), center(c), radius(r) {}
    ~Sphere() override;

    bool rayIntersect(const Point3D &orig, const Point3D &dir, double &t0) const override {
        auto L = center - orig;
        auto tca = L * dir;
        auto d2 = L * L - tca * tca;
        if (d2 > radius * radius) return false;
        auto thc = std::sqrt(radius * radius - d2);
        t0 = tca - thc;
        auto t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }

    Point3D normal(const Point3D &hit) const override {
        return (hit - center).normalize();
    }
};

#endif // UTILS_H
