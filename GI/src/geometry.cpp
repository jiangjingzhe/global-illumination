#define _USE_MATH_DEFINES
#include "geometry.h"
#include <math.h>

// Vec类方法实现
Vec::Vec(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

Vec Vec::operator+(const Vec &b) const {return Vec(x+b.x, y+b.y, z+b.z);}

Vec Vec::operator-(const Vec &b) const {return Vec(x-b.x, y-b.y, z-b.z);}

Vec Vec::operator*(double b) const {return Vec(x*b, y*b, z*b);}

Vec Vec::mult(const Vec &b) const {return Vec(x*b.x, y*b.y, z*b.z);}

Vec& Vec::norm() {
	double l=sqrt(x*x+y*y+z*z);
	x/=l;
	y/=l;
	z/=l;
	return *this;
}

double Vec::dot(const Vec &b) const {return x*b.x + y*b.y + z*b.z;}

Vec Vec::operator%(const Vec &b) const {return Vec(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);}

// Ray构造函数
Ray::Ray(Vec o_, Vec d_) : o(o_), d(d_.norm()) {}

// Sphere类方法实现
Sphere::Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_)
	: rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}

double Sphere::intersect(const Ray &r) const {
	Vec op = p - r.o;
	double t, eps=1e-4, b=op.dot(r.d), det=b*b - op.dot(op) + rad*rad;
	if (det < 0) return 0;
	det = sqrt(det);
	return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
}
