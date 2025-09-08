#pragma once

#include <algorithm>
#include <cstddef>

// Minimal raster helpers for grayscale/RGBA images.
// Coordinate system: integer pixel coords, (0,0) at top-left of the buffer.
// Bounds are clamped in the plot functions; callers should still check for speed.
namespace Raster {

struct ImageGray {
    unsigned char* data;
    int width;
    int height;
};

struct ImageRgba {
    unsigned char* data; // RGBA8, tightly packed
    int width;
    int height;
};

// Plot a single pixel
inline void plot(ImageGray img, int x, int y, unsigned char v = 255) {
    if (x < 0 || y < 0 || x >= img.width || y >= img.height) return;
    img.data[static_cast<size_t>(y * img.width + x)] = v;
}

inline void plot(ImageRgba img, int x, int y, unsigned char v = 255, unsigned char a = 255) {
    if (x < 0 || y < 0 || x >= img.width || y >= img.height) return;
    const size_t i = static_cast<size_t>((y * img.width + x) * 4);
    img.data[i + 0] = v;
    img.data[i + 1] = v;
    img.data[i + 2] = v;
    img.data[i + 3] = a;
}

// Clear
inline void clear(ImageGray img, unsigned char v = 0) {
    const size_t n = static_cast<size_t>(img.width) * static_cast<size_t>(img.height);
    std::fill(img.data, img.data + n, v);
}

inline void clear(ImageRgba img, unsigned char v = 0, unsigned char a = 255) {
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            plot(img, x, y, v, a);
        }
    }
}

// Disc
inline void drawDisc(ImageGray img, int cx, int cy, int r, unsigned char v = 255) {
    int r2 = r * r;
    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dy * dy <= r2) plot(img, cx + dx, cy + dy, v);
        }
    }
}

inline void drawDisc(ImageRgba img, int cx, int cy, int r, unsigned char v = 255, unsigned char a = 255) {
    int r2 = r * r;
    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dy * dy <= r2) plot(img, cx + dx, cy + dy, v, a);
        }
    }
}

// Bresenham line with optional radius (disc at each step)
inline void drawLine(ImageGray img, int x0, int y0, int x1, int y1, int r = 0, unsigned char v = 255) {
    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    while (true) {
        if (r > 0) drawDisc(img, x0, y0, r, v); else plot(img, x0, y0, v);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

inline void drawLine(ImageRgba img, int x0, int y0, int x1, int y1, int r = 0, unsigned char v = 255, unsigned char a = 255) {
    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    while (true) {
        if (r > 0) drawDisc(img, x0, y0, r, v, a); else plot(img, x0, y0, v, a);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

} // namespace Raster

