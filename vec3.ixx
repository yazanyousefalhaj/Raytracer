module;
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
export module vec3;
import math;

using std::sqrt, std::ostream, std::clamp;


export class Vec3;
export Vec3 unit_vector(const Vec3& v);
export double dot(const Vec3& l, const Vec3& r);
export Vec3 operator*(const Vec3& l, const Vec3& r);
export Vec3 operator*(const Vec3& l, double r);
export Vec3 operator*(double l, const Vec3& r);
export Vec3 operator-(const Vec3& l, const Vec3& r);
export Vec3 operator+(const Vec3& l, const Vec3& r);


export class Vec3 {
public:
	Vec3() : e{ 0.0, 0.0, 0.0 } 
	{ }

	Vec3(double x, double y, double z): e{ x, y, z }
	{ }

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	Vec3 operator-() const { return Vec3{ -e[0], -e[1], -e[2] }; }
	double operator[](size_t i) const { return e[i]; }
	double& operator[](size_t i) { return e[i]; }

	Vec3& operator+=(const Vec3& r)
	{
		e[0] += r.e[0];
		e[1] += r.e[1];
		e[2] += r.e[2];
		return *this;
	}

	Vec3& operator-=(const Vec3& r)
	{
		e[0] -= r.e[0];
		e[1] -= r.e[1];
		e[2] -= r.e[2];
		return *this;
	}

	Vec3& operator*=(const Vec3& r)
	{
		e[0] *= r.e[0];
		e[1] *= r.e[1];
		e[2] *= r.e[2];
		return *this;
	}

	Vec3& operator*=(double r)
	{
		e[0] *= r;
		e[1] *= r;
		e[2] *= r;
		return *this;
	}

	Vec3& operator/=(const Vec3& r)
	{
		e[0] /= r.e[0];
		e[1] /= r.e[1];
		e[2] /= r.e[2];
		return *this;
	}

	double length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}
	
	double length() const
	{
		return sqrt(length_squared());
	}

	static Vec3 random()
	{
		return Vec3{ random_double(), random_double(), random_double() };
	}

	static Vec3 random(double min, double max)
	{
		return Vec3{ random_double(min, max), random_double(min, max), random_double(min, max) };
	}

	static Vec3 random_in_unit_sphere()
	{
		while (true) {
			auto p = random(-1, 1);
			if (p.length_squared() < 1) {
				return p;
			}
		}
	}

	static Vec3 random_in_unit_disk()
	{
		while (true) {
			auto p = Vec3{ random_double(-1, 1), random_double(-1, 1), 0.0 };
			if (p.length_squared() < 1) {
				return p;
			}
		}
	}

	static Vec3 random_unit_vec()
	{
		return unit_vector(random_in_unit_sphere());
	}

	bool near_zero() const
	{
		const double s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
	}

	static Vec3 reflect(const Vec3& v, const Vec3& n)
	{
		return v - 2 * dot(v, n) * n;
	}

	static Vec3 refract(const Vec3& v, const Vec3& n, double refraction_index_ratio)
	{
		const auto cos_theta = std::min(dot(-v, n), 1.0);
		const auto r_perp = refraction_index_ratio * (v + cos_theta * n);
		const auto r_parallel = -sqrt(1 - r_perp.length_squared()) * n;
		return r_perp + r_parallel;
	}

	double e[3];
};

export using point3 = Vec3;
export using color3 = Vec3;


export ostream& operator<<(ostream& out, Vec3 &vec)
{
	return out << vec.x() << ", " << vec.y() << ", " << vec.z();
}

export Vec3 operator+(const Vec3& l, const Vec3& r)
{
	auto copy = l;
	copy += r;
	return copy;
}


export Vec3 operator-(const Vec3& l, const Vec3& r)
{
	auto copy = l;
	copy -= r;
	return copy;
}

export Vec3 operator*(const Vec3& l, const Vec3& r)
{
	auto copy = l;
	copy *= r;
	return copy;
}

export Vec3 operator*(const Vec3& l, double r)
{
	auto copy = l;
	copy *= r;
	return copy;
}

export Vec3 operator*(double l, const Vec3& r)
{
	auto copy = r;
	copy *= l;
	return copy;
}

export Vec3 operator/(const Vec3& l, double r)
{
	return (1 / r) * l;
}

export double dot(const Vec3& l, const Vec3& r)
{
	return l[0] * r[0] + l[1] * r[1] + l[2] * r[2];
}

export Vec3 cross(const Vec3& l, const Vec3& r)
{
	return Vec3(
		l[1] * r[2] - l[2] * r[1],
		l[2] * r[0] - l[0] * r[2],
		l[0] * r[1] - l[1] * r[0]
	);
}

export Vec3 unit_vector(const Vec3& v)
{
	return v / v.length();
}

export void write_color(std::vector<std::byte>& data, size_t image_width, size_t h, size_t w, const Vec3& color, size_t samples_per_pixal)
{
	auto scale = 1.0 / samples_per_pixal;
	data[h * image_width * 3 + w * 3] = static_cast<std::byte>(clamp(sqrt(color[0] * scale), 0.0, 1.0) * 255.99999);
	data[h * image_width * 3 + w * 3 + 1] = static_cast<std::byte>(clamp(sqrt(color[1] * scale), 0.0, 1.0) * 255.99999);
	data[h * image_width * 3 + w * 3 + 2] = static_cast<std::byte>(clamp(sqrt(color[2] * scale), 0.0, 1.0) * 255.99999);
}
