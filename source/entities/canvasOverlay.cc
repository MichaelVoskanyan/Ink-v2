#include "canvasOverlay.h"
#include "renderer/textureManager.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include "core/strokeRecorder.h"
#include "core/drawing.h"

#include <cassert>
#include <memory>
#include <algorithm>
#include <cmath>

using std::make_shared;

namespace {
    constexpr int kW = 64;
    constexpr int kH = 64;
}  // namespace

// Create a small quad with default scale and a position likely visible with the
// current camera. Defer GL allocations to ensureInitialized(). Disable the
// hitbox so this overlay never participates in collision.
CanvasOverlay::CanvasOverlay() : GameObject(glm::vec2(0.2f, 0.2f), glm::vec3(-0.8f, 0.8f, 0.0f)) {
    hitbox.setActive(false);
    ensureInitialized();
}

// Populate m_pixels with a grayscale checkerboard for easy visual validation.
void CanvasOverlay::fillChecker(unsigned char a, int phase) {
    if (m_pixels.size() != static_cast<size_t>(kW * kH * 4))
        m_pixels.resize(kW * kH * 4);
    for (int y = 0; y < kH; ++y) {
        for (int x = 0; x < kW; ++x) {
            bool on = ((x / 8 + y / 8 + phase) % 2) == 0;
            unsigned char c = on ? 255 : 30;
            size_t i = static_cast<size_t>((y * kW + x) * 4);
            m_pixels[i + 0] = c;
            m_pixels[i + 1] = c;
            m_pixels[i + 2] = c;
            m_pixels[i + 3] = a;
        }
    }
}

void CanvasOverlay::clearCanvas(unsigned char value, unsigned char a) {
    std::lock_guard<std::mutex> lk(m_pixelsMutex);
    if (m_pixels.size() != static_cast<size_t>(kW * kH * 4))
        m_pixels.resize(kW * kH * 4);
    for (int y = 0; y < kH; ++y) {
        for (int x = 0; x < kW; ++x) {
            size_t i = static_cast<size_t>((y * kW + x) * 4);
            m_pixels[i + 0] = value;
            m_pixels[i + 1] = value;
            m_pixels[i + 2] = value;
            m_pixels[i + 3] = a;
        }
    }
}

void CanvasOverlay::setPixel(int x, int y, unsigned char v, unsigned char a) {
    Raster::ImageRgba img{m_pixels.data(), kW, kH};
    Raster::plot(img, x, y, v, a);
}

void CanvasOverlay::drawDisc(int cx, int cy, int radius, unsigned char v, unsigned char a) {
    Raster::ImageRgba img{m_pixels.data(), kW, kH};
    Raster::drawDisc(img, cx, cy, radius, v, a);
}

void CanvasOverlay::drawLine(int x0, int y0, int x1, int y1, int radius, unsigned char v,
                             unsigned char a) {
    Raster::ImageRgba img{m_pixels.data(), kW, kH};
    Raster::drawLine(img, x0, y0, x1, y1, radius, v, a);
}

std::pair<int, int> CanvasOverlay::toCanvas(float nx, float ny) const {
    // Clamp to [0,1] then scale to pixel indices.
    // StrokeRecorder provides window-normalized coords with origin top-left (y down).
    // Textures in OpenGL sample with v=0 at bottom, so flip Y here to match display.
    nx = std::max(0.0f, std::min(1.0f, nx));
    ny = std::max(0.0f, std::min(1.0f, ny));
    int x = static_cast<int>(nx * (kW - 1) + 0.5f);
    // why only y needs to be flipped?
    int y = static_cast<int>((1.0f - ny) * (kH - 1) + 0.5f);
    return {x, y};
}

// Allocate the dynamic texture and build a basic textured quad (VAO/VBO/IBO + shader).
void CanvasOverlay::ensureInitialized() {
    if (m_init)
        return;

    auto tm = TextureManager::instance();
    // Initialize to a blank canvas
    m_pixels.resize(kW * kH * 4);
    clearCanvas(0, 255);
    tm->createDynamicTexture("draw_canvas", kW, kH, m_pixels.data());

    // Simple textured quad (positions, normals, uvs)
    float verts[] = {-0.5f, -0.5f, 0.0f, 0,     0,    1,    0.0f, 0.0f, -0.5f, 0.5f, 0.0f,
                     0,     0,     1,    0.0f,  1.0f, 0.5f, 0.5f, 0.0f, 0,     0,    1,
                     1.0f,  1.0f,  0.5f, -0.5f, 0.0f, 0,    0,    1,    1.0f,  0.0f};
    unsigned int idx[] = {0, 1, 2, 0, 2, 3};

    renderObject = make_shared<SceneObject>();
    renderObject->m_mesh = make_shared<Mesh>();
    renderObject->m_mesh->m_vertexArray =
            make_shared<VertexArray>(make_shared<VertexBuffer>(verts, sizeof(verts)),
                                     make_shared<IndexBuffer>(idx, sizeof(idx) / sizeof(idx[0])));
    renderObject->m_mesh->m_shader = make_shared<Shader>("char.vs", "char.fs");
    renderObject->m_mesh->m_texture = tm->getTexture("draw_canvas");

    // Use GameObject's initial position/scale
    renderObject->m_transform.m_position = position;
    renderObject->m_transform.m_scale = glm::vec3(scale, 1.0f);

    renderObject->m_screenSpace = true;
    m_init = true;
}

// Advance animation and update the CPU buffer. GPU upload is done on the
// render thread via uploadToGpu() to respect GL context ownership.
void CanvasOverlay::update(float dt) {
    ensureInitialized();

    // Snapshot current stroke points (normalized coords) safely
    auto pts = StrokeRecorder::instance()->copyCurrentPoints();

    // Redraw the canvas from scratch each tick: black background + white stroke
    {
        std::lock_guard<std::mutex> lk(m_pixelsMutex);
        // Clear to dark background for contrast
        Raster::ImageRgba img{m_pixels.data(), kW, kH};
        Raster::clear(img, 16, 255);

        // Draw stroke if there are points
        const int radius = 1;  // ~2-3px thickness
        if (!pts.empty()) {
            auto [x0, y0] = toCanvas(pts[0].x, pts[0].y);
            drawDisc(x0, y0, radius);
            for (size_t i = 1; i < pts.size(); ++i) {
                auto [x1, y1] = toCanvas(pts[i].x, pts[i].y);
                drawLine(x0, y0, x1, y1, radius);
                x0 = x1;
                y0 = y1;
            }
        }
    }

    // Keep transform in sync (in case caller changed position/scale)
    if (renderObject) {
        renderObject->m_transform.m_position = position;
        renderObject->m_transform.m_scale = glm::vec3(scale, 1.0f);
    }
}

// Upload the current CPU pixels to the GPU texture (call on render thread).
void CanvasOverlay::uploadToGpu() {
    std::lock_guard<std::mutex> lk(m_pixelsMutex);
    TextureManager::instance()->updateDynamicTexture("draw_canvas", m_pixels.data(), false);
}

// No-op: Renderer submits our SceneObject; nothing to do here.
void CanvasOverlay::draw() {
    // Rendering is handled by Renderer via SceneObject submission.
    return;
}
