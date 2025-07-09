/**********************************************************/
//
// File: main.cpp
//
// Purpose: main entry point
//
// Project: raytrace
//
/**********************************************************/

#pragma once
#include NAMES_INCLUDE
#include STL_INCLUDE_PATH


// Simple 3D Vector class
struct Vec3 {
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double a, double b, double c) : x(a), y(b), z(c) {}

    Vec3 operator + (const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator - (const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator * (double t) const { return Vec3(x * t, y * t, z * t); }

    double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const {
        double mag = std::sqrt(x * x + y * y + z * z);
        return Vec3(x / mag, y / mag, z / mag);
    }
};

// Ray = origin + t * direction
struct Ray {
    Vec3 origin, direction;
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
};

// Returns true if ray hits sphere
bool hit_sphere(const Vec3& center, double radius, const Ray& r) {
    Vec3 oc = r.origin - center;
    double a = r.direction.dot(r.direction);
    double b = 2.0 * oc.dot(r.direction);
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

// Color function
Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - n * 2 * v.dot(n);
}

Vec3 ray_color(const Ray& r, int depth) {
    if (depth <= 0)
        return Vec3(0, 0, 0); // No more light is gathered

    Vec3 sphere_center(0, 0, -1);
    double sphere_radius = 0.5;

    Vec3 oc = r.origin - sphere_center;
    double a = r.direction.dot(r.direction);
    double b = 2.0 * oc.dot(r.direction);
    double c = oc.dot(oc) - sphere_radius * sphere_radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        double t = (-b - std::sqrt(discriminant)) / (2.0 * a);
        if (t > 0) {
            Vec3 hit_point = r.origin + r.direction * t;
            Vec3 normal = (hit_point - sphere_center).normalize();

            // Reflected ray
            Vec3 reflected_dir = reflect(r.direction.normalize(), normal);
            Ray reflected_ray(hit_point, reflected_dir);

            // Recursive color from reflection
            Vec3 reflected_color = ray_color(reflected_ray, depth - 1);

            // Basic material: 80% reflection + 20% red
            Vec3 local_color(1, 0, 0);
            Vec3 color = local_color * 0.2 + reflected_color * 0.8;
            return color;
        }
    }

    // Background gradient
    Vec3 unit_direction = r.direction.normalize();
    double t = 0.5 * (unit_direction.y + 1.0);
    return Vec3(1.0, 1.0, 1.0) * (1.0 - t) + Vec3(0.5, 0.7, 1.0) * t;
}

#define MAX_LIGHT_BOUNCES 10

int main() {
    const int image_width = 4000;
    const int image_height = 2000;

    std::ofstream out("output.ppm");
    out << "P3\n" << image_width << " " << image_height << "\n255\n";

    Vec3 origin(0, 0, 0);
    double viewport_height = 2.0;
    double viewport_width = 4.0;
    double focal_length = 1.0;

    Vec3 horizontal(viewport_width, 0, 0);
    Vec3 vertical(0, viewport_height, 0);
    Vec3 lower_left_corner = origin - horizontal * 0.5 - vertical * 0.5 - Vec3(0, 0, focal_length);

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);

            Vec3 direction = lower_left_corner + horizontal * u + vertical * v - origin;
            Ray r(origin, direction);

            Vec3 color = ray_color(r, MAX_LIGHT_BOUNCES);

            int ir = static_cast<int>(255.99 * color.x);
            int ig = static_cast<int>(255.99 * color.y);
            int ib = static_cast<int>(255.99 * color.z);

            out << ir << " " << ig << " " << ib << "\n";
        }
    }

    out.close();
    std::cout << "Done! Output written to output.ppm\n";
    return 0;
}
