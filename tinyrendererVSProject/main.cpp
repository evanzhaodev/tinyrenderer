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

Vec3f barycentric(Vec2i* pts, Vec2i& P) {
    Vec3f coord = Vec3f(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x) 
        ^ Vec3f(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y);
    if (std::abs(coord.z) < 1)
        return Vec3f(-1, -1, -1);
    return Vec3f(1.f - (coord.x + coord.y) / coord.z, coord.x / coord.z, coord.y / coord.z);
}

void triangle(Vec2i* pts, TGAImage& image, const TGAColor& color) {
    Vec2i bboxmin(image.width() - 1, image.height() - 1);
    Vec2i bboxmax(0, 0);

    for (int i = 0; i < 3; i++) {
        bboxmax.x = std::max(bboxmax.x, pts[i].x);
        bboxmax.y = std::max(bboxmax.y, pts[i].y);
        bboxmax.x = std::min(bboxmax.x, image.width() - 1);
        bboxmax.y = std::min(bboxmax.y, image.height() - 1);

        bboxmin.x = std::min(bboxmin.x, pts[i].x);
        bboxmin.y = std::min(bboxmin.y, pts[i].y);
        bboxmin.x = std::max(bboxmin.x, 0);
        bboxmin.y = std::max(bboxmin.y, 0);
    }

    Vec2i P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f barycentricCoord = barycentric(pts, P);
            if (barycentricCoord.x < 0 || barycentricCoord.y < 0 || barycentricCoord.z < 0)
                continue;

            image.set(P.x, P.y, color);
        }
    }
}

int main(int argc, char** argv) {
    TGAImage frame(200, 200, TGAImage::RGB);
    Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
    triangle(pts, frame, red);
    frame.write_tga_file("framebuffer.tga");
    return 0;
}