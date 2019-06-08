#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <cassert>
#include <initializer_list>

template <size_t D> class Point {
    double data[D];

public:
    Point() {
        for (auto i = D; i--; data[i] = 0.);
    }

    Point(std::initializer_list<double> list) {
        auto i = 0;
        for (auto elem: list) {
            data[i] = elem;
            i++;
        }
    }

    double & operator[](const size_t i) {
        assert(i < D);
        return data[i];
    }

    const double & operator[](const size_t i) const {
        assert(i < D);
        return data[i];
    }

    double norm() {
        auto rez = 0.;
        for (auto i = D; i--; rez += data[i] * data[i]);
        return std::sqrt(rez);
    }

    Point & normalize(double l = 1.) {
        *this = (*this) * (l/norm());
        return *this;
    }
};

template<size_t D> double operator*(const Point<D> &lhs, const Point<D> &rhs) {
    auto rez = 0.;
    for (auto i = D; i--; rez += lhs[i] * rhs[i]);
    return rez;
}

template<size_t D> Point<D> operator+(Point<D> lhs, const Point<D> &rhs) {
    for (auto i = D; i--; lhs[i] += rhs[i]);
    return lhs;
}

template<size_t D> Point<D> operator-(Point<D> lhs, const Point<D> &rhs) {
    for (auto i = D; i--; lhs[i] -= rhs[i]);
    return lhs;
}

template<size_t D, typename U> Point<D> operator*(const Point<D> &lhs, const U &rhs) {
    Point<D> rez;
    for (size_t i = D; i--; rez[i] = lhs[i] * rhs);
    return rez;
}

template<size_t D> Point<D> operator-(const Point<D> &lhs) {
    return lhs * (-1);
}

typedef Point<3> Point3D;
typedef Point<4> Point4D;

#endif // GEOMETRY_H
