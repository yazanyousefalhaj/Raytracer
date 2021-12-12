module;
#include <optional>
#include <utility>
#include <numeric>
#include <cmath>
export module material;
import ray;
import vec3;
import hittable;
import math;

using std::nullopt, std::optional;


struct ScatterResult
{
	std::optional<Ray> scattered;
	std::optional<color3> attenuation;
};


export class Material
{
public:
	virtual ScatterResult scatter(const Ray& r, const HitRecord& hit_record) = 0;
};


export class Lambertian : public Material
{
public:
	Lambertian(color3 _albedo): albedo{_albedo}
	{}

	ScatterResult scatter(const Ray& r, const HitRecord& hit_record) override
	{
		auto scatter_direction = hit_record.normal + Vec3::random_unit_vec();
		if (scatter_direction.near_zero()) {
			scatter_direction = hit_record.normal;
		}
		auto scattered = Ray{ hit_record.p, scatter_direction };
		return {
			.scattered = scattered,
			.attenuation = albedo,
		};
	}

	color3 albedo;
};


export class Metal : public Material
{
public:
	Metal(color3 _albedo, double f): albedo{_albedo}, fuzz{f < 1? f: 1}
	{}

	virtual ScatterResult scatter(const Ray& r, const HitRecord& hit_record) override
	{
		Vec3 reflected = Vec3::reflect(unit_vector(r.dir), hit_record.normal);
		auto scattered = Ray(hit_record.p, reflected + fuzz * Vec3::random_in_unit_sphere());
		if (dot(scattered.direction(), hit_record.normal) > 0) {
			return {
				.scattered = scattered,
				.attenuation = albedo,
			};
		}
		return { nullopt, nullopt };
	}

	color3 albedo;
	double fuzz;
};


export class Dielectric : public Material
{
public:
	Dielectric(double _refraction_index) :refraction_index{_refraction_index}
	{ }

	ScatterResult scatter(const Ray& r, const HitRecord& hit_record) override
	{
		double refraction_ratio = hit_record.front_face ? (1.0 / refraction_index) : refraction_index;
		const auto unit_dir = unit_vector(r.direction());

		const auto cos_theta = std::min(dot(-unit_dir, hit_record.normal), 1.0);
		const auto sin_theta = sqrt(1 - cos_theta * cos_theta);
		Vec3 direction;
		if (refraction_ratio * sin_theta > 1 || reflectance(cos_theta) > random_double()) {
			direction = Vec3::reflect(unit_dir, hit_record.normal);
		} else {
			direction = Vec3::refract(unit_dir, hit_record.normal, refraction_ratio);
		}

		return {
			.scattered = Ray(hit_record.p, direction),
			.attenuation = color3(1.0, 1.0, 1.0),
		};
	}

private:
	double refraction_index;

	double reflectance(double cosine)
	{
		auto r0 = (1 - this->refraction_index) / (1 - this->refraction_index);
		r0 *= r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};
