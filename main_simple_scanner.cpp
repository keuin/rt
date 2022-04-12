//
// Created by Keuin on 2022/4/11.
//

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>

#include "vec.h"
#include "ray.h"
#include "bitmap.h"
#include "timer.h"

#define DEMO_BALL

class object {
public:
    // Will the given ray hit.
    virtual bool hit(const ray3d &r) const = 0;

    // object color, currently not parameterized
    virtual pixel8b color() const = 0;

    // subclasses must have virtual destructors
    virtual ~object() = default;
};

class sphere : public object {
    vec3d center;
    double radius;

public:
    sphere() = delete;

    sphere(const vec3d &center, double radius) : center(center), radius(radius) {}

    ~sphere() override = default;

    bool hit(const ray3d &r) const override {
        // Ray: {Source, Direction, time}
        // Sphere: {Center, radius}
        // sphere hit formula: |Source + Direction * time - Center| = radius
        // |(Sx + Dx * t - Cx, Sy + Dy * t - Cy, Sz + Dz * t - Cz)| = radius

        // A = D dot D
        const double a = dot(r.direction(), r.direction());
        // B = 2(S - C) dot D
        const double b = 2 * dot(r.source() - center, r.direction());
        const auto rel_src = r.source() - center; // relative position of ray source
        // C = (S - C) dot (S - C) - radius^2
        const double c = dot(rel_src, rel_src) - radius * radius;
        return b * b - 4 * a * c >= 0;
    }

    pixel8b color() const override {
        return pixel8b::from_normalized(1.0, 0.0, 0.0);
    }
};

class viewport {
    double half_width, half_height; // viewport size
    vec3d center; // coordinate of the viewport center point
    std::vector<std::unique_ptr<object>> objects;

    // Given a ray from the camera, generate a color the camera seen on the viewport.
    pixel8b color(const ray3d &r) {
        // Detect hits
        for (const auto &obj: objects) {
            if (obj->hit(r)) {
                return obj->color();
            }
        }

        // Does not hit anything. Get background color (infinity)
        const auto u = (r.direction().y + 1.0) * 0.5;
        return mix(
                pixel8b::from_normalized(1.0, 1.0, 1.0),
                pixel8b::from_normalized(0.5, 0.7, 1.0),
                1.0 - u,
                u
        );
    }

public:
    viewport() = delete;

    viewport(double width, double height, vec3d viewport_center) :
            half_width(width / 2.0), half_height(height / 2.0), center(viewport_center) {}

    // Add an object to the world.
    void add_object(std::unique_ptr<object> &&obj) {
        objects.push_back(std::move(obj));
    }

    // Generate the image seen on given viewpoint.
    bitmap8b render(vec3d viewpoint, uint16_t image_width, uint16_t image_height) {
        bitmap8b image{image_width, image_height};
        const auto r = center - viewpoint;
        const int img_hw = image_width / 2, img_hh = image_height / 2;
        // iterate over every pixel on the image
        for (int j = -img_hh; j < img_hh; ++j) { // axis y
            for (int i = -img_hw; i < img_hw; ++i) { // axis x
                const vec3d off{
                        .x=1.0 * i / img_hw * half_width,
                        .y=1.0 * j / img_hh * half_height,
                        .z=0.0
                }; // offset on screen plane
                const auto dir = r + off; // direction vector from camera to current pixel on screen
                const ray3d ray{viewpoint, dir}; // from camera to pixel (on the viewport)
                const auto pixel = color(ray);
                image.set(i + img_hw, j + img_hh, pixel);
            }
        }
        return image;
    }
};

void generate_image(uint16_t image_width, uint16_t image_height, double viewport_width, double focal_length,
                    double sphere_z, double sphere_r) {
    double r = 1.0 * image_width / image_height;
    viewport vp{viewport_width, viewport_width / r, vec3d{0, 0, -focal_length}};
    vp.add_object(std::unique_ptr<object>{new sphere{vec3d{0, 0, sphere_z}, sphere_r}});
    timer tm;
    tm.start_measure();
    const auto image = vp.render(vec3d::zero(), image_width, image_height); // camera position as the coordinate origin
    tm.stop_measure();
    if (!std::getenv("NOPRINT")) {
        image.write_plain_ppm(std::cout);
    } else {
        std::cerr << "NOPRINT is defined. PPM Image won't be printed." << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 7) {
        printf("Usage: %s <image_width> <image_height> <viewport_width> <focal_length> <sphere_z> <sphere_r>\n",
               argv[0]);
        return 0;
    }
    std::string iw{argv[1]}, ih{argv[2]}, vw{argv[3]}, fl{argv[4]}, sz{argv[5]}, sr{argv[6]};
    generate_image(std::stoul(iw), std::stoul(ih),
                   std::stod(vw), std::stod(fl),
                   std::stod(sz), std::stod(sr));
}