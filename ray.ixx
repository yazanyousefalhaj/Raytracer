export module ray;
import vec3;


export class Ray
{
public:
	Ray(const point3 origin, const Vec3& direction): orig{origin}, dir{direction}
	{ }

	point3 origin() const { return orig;  }
	Vec3 direction() const { return dir;  }

	Vec3 at(const double t) const
	{
		return orig + t * dir;
	}

	point3 orig;
	Vec3 dir;
};