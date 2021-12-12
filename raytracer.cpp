#include <cstdio>
#include <vector>
#include <cmath>
#include <optional>
#include <memory>
#include <map>
#include <string>
#include <algorithm>
#include <execution>
#include <range/v3/all.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include "stbi_image_write.h"
import vec3;
import ray;
import hittable;
import math;
import camera;
import material;

using std::optional, std::vector, std::byte, std::make_unique, std::map, std::string, std::unique_ptr, std::move;


static std::map<string, unique_ptr<Material>> materials{};


HittableList random_scene() {
    HittableList world;

    materials["ground_material"] = make_unique<Lambertian>(color3(0.5, 0.5, 0.5));
    world.add_object(make_unique<Sphere>(point3(0, -1000, 0), 1000, materials["ground_material"].get()));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const auto material_index = fmt::format("{}_{}", a, b);
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::unique_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color3::random() * color3::random();
                    sphere_material = make_unique<Lambertian>(albedo);
                    world.add_object(make_unique<Sphere>(center, 0.2, sphere_material.get()));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_unique<Metal>(albedo, fuzz);
                    world.add_object(make_unique<Sphere>(center, 0.2, sphere_material.get()));
                } else {
                    // glass
                    sphere_material = make_unique<Dielectric>(1.5);
                    world.add_object(make_unique<Sphere>(center, 0.2, sphere_material.get()));
                }
                materials[material_index] = move(sphere_material);
            }
        }
    }

    materials["material1"] = make_unique<Dielectric>(1.5);
    world.add_object(make_unique<Sphere>(point3(0, 1, 0), 1.0, materials["material1"].get()));

    materials["material2"] = make_unique<Lambertian>(color3(0.4, 0.2, 0.1));
    world.add_object(make_unique<Sphere>(point3(-4, 1, 0), 1.0, materials["material2"].get()));

    materials["material3"] = make_unique<Metal>(color3(0.7, 0.6, 0.5), 0.0);
    world.add_object(make_unique<Sphere>(point3(4, 1, 0), 1.0, materials["material3"].get()));

    return world;
}


const color3 ray_color(const Ray& ray, HittableList& world, int depth = 50)
{
    if (depth <= 0) { return color3(0.0, 0.0, 0.0); }
    if (auto t = world.hit(ray, 0.001, infinity)) {
        auto res = t.value().mat->scatter(ray, t.value());
        if (res.scattered) {
            return res.attenuation.value() * ray_color(res.scattered.value(), world, depth - 1);
        }
        return color3{ 0.0, 0.0, 0.0 };
    }

    auto t = 0.5 * (unit_vector(ray.direction()).y() + 1.0);
    return (1 - t) * color3 { 1.0, 1.0, 1.0 } + t * color3{ 0.5, 0.7, 1.0 };
}

int main()
{
    point3 lookfrom{ 13, 2, 3 };
    point3 lookat{ 0, 0, 0 };
    Camera camera{ lookfrom, lookat, Vec3{0, 1, 0}, 90.0, 0.1,  10.0 };
    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 1200;
    constexpr int image_height = static_cast<int>(image_width / Camera::aspect_ratio);
    constexpr size_t samples_per_pixal = 500;

    auto world = random_scene();

    vector<byte> data(image_height * image_width * 3);

    auto ints = ranges::views::reverse(ranges::views::iota(0, image_height));

    std::for_each(std::execution::par_unseq, ints.begin(), ints.end(), [&](auto i) {
        fmt::print("Working on: {}   \n", i);
        for (auto j = 0; j < image_width; ++j) {
            auto color = color3{ 0.0, 0.0, 0.0 };
            for (size_t s = 0; s < samples_per_pixal; ++s) {
				auto u = double(j + random_double()) / (image_width - 1);
				auto v = double(image_height - i + random_double()) / (image_height - 1);

				const Ray ray = camera.get_ray(u, v);
				color += ray_color(ray, world);
            }
			write_color(data, image_width, i, j, color, samples_per_pixal);
        }
    });

    stbi_write_png("image.png", image_width, image_height, 3, data.data(), 0);

    fmt::print(stderr, "\nDone\n");
}

