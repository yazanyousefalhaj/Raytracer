module;
#include <limits>
#include <random>
export module math;


export inline constexpr double infinity = std::numeric_limits<double>::infinity();
export inline constexpr double pi = 3.1415926535897932385;

export double degrees_to_radians(double degree)
{
	return degree / 180.0;
}

export double random_double()
{
	static std::uniform_real_distribution dist{0.0, 1.0};
	static std::mt19937 gen{};
	return dist(gen);
}

export double random_double(double min, double max)
{
	return min + random_double() * (max - min);
}

