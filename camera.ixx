module;
#include <cmath>
export module camera;
import vec3;
import ray;
import math;


export class Camera
{
public:
	Camera(point3 lookfrom, point3 lookat, Vec3 vup, double fov, double aperture, double focus_dist)
	{
		auto theta = degrees_to_radians(fov);
		auto h = std::tan(theta / 2.0);
		double view_port_height = 2.0 * h;
		double view_port_width = view_port_height * aspect_ratio;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = unit_vector(cross(w, u));

		origin = lookfrom;
		horizontal = focus_dist * view_port_width * u;
		vertical = focus_dist * view_port_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;
		
		lens_radius = aperture / 2;
	}

	Ray get_ray(double s, double t)
	{
		Vec3 rd = lens_radius * Vec3::random_in_unit_disk();
		Vec3 offset = u * rd.x() + v * rd.y();
		return Ray(origin + offset, lower_left_corner + horizontal * s + vertical * t - origin - offset);
	}

	point3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 w;
	Vec3 u;
	Vec3 v;
	double lens_radius;


	static constexpr double aspect_ratio = 16.0 / 9.0;
	static constexpr double focal_length = 1.0;
};