module;
#include <optional>
#include <cmath>
#include <memory>
#include <vector>
export module hittable;
import vec3;
import ray;

using std::optional, std::nullopt, std::vector, std::unique_ptr, std::move;


export class Material;


export struct HitRecord
{
	point3 p;
	Vec3 normal;
	double t;
	bool front_face;
	Material* mat;

	HitRecord(point3 _p, Vec3 _normal, double _t, const Ray& ray, Material* _mat) : p{ _p }, normal{ _normal }, t{ _t }, mat{_mat}
	{
		front_face = dot(_normal, ray.direction()) < 0;
		normal = front_face ? normal: -normal;
	}
};


export class Hittable
{
public:
	Hittable()
	{ }

	virtual ~Hittable()
	{ }

	virtual const optional<HitRecord> hit(const Ray& ray, double t_min, double t_max) const = 0;
};


export class Sphere : public Hittable
{
public:
	Sphere(point3 c, double r, Material* mat): center{c}, radius{r}, material{mat}
	{ }

	const optional<HitRecord> hit(const Ray& ray, double t_min, double t_max) const override
	{
		auto oc = ray.origin() - center;
		auto a = ray.direction().length_squared();
		auto b = dot(ray.direction(), oc);
		auto c = oc.length_squared() - radius * radius;
		auto d = b * b - a * c;

		if (d < 0) {
			return nullopt;
		} else {
			auto t = (-b - std::sqrt(d)) / a;
			if (t < t_min || t > t_max) {
				t = (-b + std::sqrt(d)) / a;;
				if (t < t_min || t > t_max) {
					return nullopt;
				}
			}
			auto p = ray.at(t);
			auto normal = unit_vector(p - center);
			return HitRecord{ p, normal, t, ray, material};
		}
	}

	point3 center;
	double radius;
	Material* material;
};


export class HittableList : public Hittable
{
public:
	void add_object(unique_ptr<Hittable> obj) { objects.push_back(move(obj)); }
	void clear() { objects.clear(); }
	

	const optional<HitRecord> hit(const Ray& ray, double t_min, double t_max) const override
	{
		optional<HitRecord> rec;
		bool hit_anything = false;
		double closest = t_max;

		for (const auto& obj : objects) {
			if (auto temp = obj->hit(ray, t_min, closest)) {
				closest = temp.value().t;
				rec = temp;
			}
		}

		return rec;
	}

	vector<unique_ptr<Hittable>> objects;
};