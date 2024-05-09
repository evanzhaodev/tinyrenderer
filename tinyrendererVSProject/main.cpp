#include "tgaimage.hpp"
#include "model.hpp"
#include <cmath>

const TGAColor white = TGAColor{ 255, 255, 255, 255 };
const TGAColor red = TGAColor{ 0, 0, 255, 255 };

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
    for (int i = 0; i < model->nfaces(); i++) {
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
    }

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}