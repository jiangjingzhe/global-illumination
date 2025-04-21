#pragma once
#include "utils.h"

enum Refl_t { DIFF, SPEC, REFR };

struct Vec {
    double x, y, z;
    Vec(double x_=0, double y_=0, double z_=0);
    Vec operator+(const Vec &b) const;
    Vec operator-(const Vec &b) const;
    Vec operator*(double b) const;
    Vec operator/(double b) const;
    Vec mult(const Vec &b) const;
    Vec& norm();
    double dot(const Vec &b) const;
    Vec operator%(const Vec &b) const;
};

struct Ray {
    Vec o, d; //起点和方向
    Ray(Vec o_, Vec d_);
};

struct Sphere {
    double rad;  //半径
    Vec p, e, c; //发光体位置，发光体颜色，物体颜色
    Refl_t refl; //反射类型
    Sphere() : rad(0), p(), e(), c(), refl(DIFF) {}
    Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_);
    double intersect(const Ray &r) const;
};

Vec randomPointOnLight(unsigned short *Xi, const Sphere &light);