#include "tgaimage.hpp"
#include "model.hpp"
#include <cmath>

const TGAColor white = TGAColor{ 255, 255, 255, 255 };
const TGAColor red = TGAColor{ 0, 0, 255, 255 };
const TGAColor green = TGAColor{ 0, 255, 0, 255 };

void line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) { // if the line is steep, we transpose the image 
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) { // make it left−to−right 
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float dydx = 0.f;
    if (x1 != x0)  // division by zero fix
        dydx = 1.f * (y1 - y0) / (x1 - x0);

    float y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color); // if transposed, de−transpose 
        }
        else {
            image.set(x, y, color);
        }
        y += dydx;
    }
}

float Vec2Distance(Vec2i v0, Vec2i v1) {
    return std::sqrtf((v1.x - v0.x) * (v1.x - v0.x) + (v1.y - v0.y) * (v1.y - v0.y));
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color) {
    if (t0.x > t1.x) {
        std::swap(t1, t0);
    }
    if (t0.x > t2.x) {
        std::swap(t2, t0);
    }
    if (t1.x > t2.x) {
        std::swap(t2, t1);
    }

    for (int x = t0.x; x <= t1.x; x++) {
        int y1 = t0.y + 1.f * (x - t0.x) * (t1.y - t0.y) / (t1.x - t0.x);
        int y2 = t0.y + 1.f * (x - t0.x) * (t2.y - t0.y) / (t2.x - t0.x);

        line(x, y1, x, y2, image, color);
    }

    for (int x = t2.x; x >= t1.x; x--) {
        int y1 = t2.y - 1.f * (t2.x - x) * (t2.y - t1.y) / (t2.x - t1.x);
        int y2 = t2.y - 1.f * (t2.x - x) * (t2.y - t0.y) / (t2.x - t0.x);

        line(x, y1, x, y2, image, color);
    }
}

int main(int argc, char** argv) {
    Model* model = NULL;
    const int width = 800;
    const int height = 800;

    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    /*for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }*/

    Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}