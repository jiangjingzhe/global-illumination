#define _USE_MATH_DEFINES
#include "geometry.h"
#include <math.h>

const float EPSILON = 1e-4f;

// Vec类方法实现
Vec::Vec(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

Vec Vec::operator+(const Vec &b) const {return Vec(x+b.x, y+b.y, z+b.z);}

Vec Vec::operator-(const Vec &b) const {return Vec(x-b.x, y-b.y, z-b.z);}

Vec Vec::operator*(double b) const {return Vec(x*b, y*b, z*b);}

Vec Vec::operator/(double b) const {return Vec(x/b, y/b, z/b);}

Vec Vec::mult(const Vec &b) const {return Vec(x*b.x, y*b.y, z*b.z);}

Vec& Vec::norm() {
	double l=sqrt(x*x+y*y+z*z);
	x/=l;
	y/=l;
	z/=l;
	return *this;
}

// 计算点积
double Vec::dot(const Vec &b) const {return x*b.x + y*b.y + z*b.z;}

// 计算叉积
Vec Vec::operator%(const Vec &b) const {return Vec(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);}

// Ray构造函数
Ray::Ray(Vec o_, Vec d_) : o(o_), d(d_.norm()) {}

// Sphere类方法实现
Sphere::Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_)
	: rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}

double Sphere::intersect(const Ray &r) const {
    Vec op = p - r.o;
    float b = op.dot(r.d);
    float det = b * b - op.dot(op) + rad*rad;
    
    if (det < 0) return 0;
    det = sqrt(det);
    
    float t = b - det;
    if (t > EPSILON) return t;
    
    t = b + det;
    return (t > EPSILON) ? t : 0;
}

Vec randomPointOnLight(unsigned short *Xi, const Sphere &light) {
    // 在光源表面均匀采样
    double theta = 2 * M_PI * erand48(Xi);
    double phi = acos(1 - 2 * erand48(Xi));
    double x = sin(phi) * cos(theta);
    double y = sin(phi) * sin(theta); 
    double z = cos(phi);
    
    Vec dir(x, y, z);
    return light.p + dir * light.rad;
}