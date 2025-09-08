#pragma once

#include "gameObject.h"
#include <memory>
#include <vector>
#include <mutex>

// A tiny on-screen quad that displays a dynamic 64x64 RGBA texture.
// Used to validate the dynamic texture pipeline (M0) and will later
// show the live drawing canvas during stroke capture (M1–M2).
class CanvasOverlay : public GameObject {
public:
    // Constructs the overlay with a small scale and a sensible default position.
    // Creates GPU resources lazily on first update via ensureInitialized().
    CanvasOverlay();

    // Per-frame tick: animates a simple checker pattern and uploads it to the
    // GPU using TextureManager::updateDynamicTexture(). Keeps the transform in sync.
    void update(float dt) override;

    // No direct GL calls here; the Renderer consumes our SceneObject.
    void draw() override;

    // Must be called on the render thread (the thread that owns the GL ctx).
    // Uploads the current CPU pixel buffer to the GPU texture.
    void uploadToGpu();

private:
    // Allocates the CPU pixel buffer, creates the GPU texture via
    // TextureManager::createDynamicTexture(), and builds a quad mesh+shader.
    void ensureInitialized();

    // Writes a 2-color checkerboard into m_pixels. 'phase' toggles the pattern
    // to demonstrate per-frame updates; 'a' sets the alpha channel.
    void fillChecker(unsigned char a = 255, int phase = 0);

    // Clear the canvas to a solid color (grayscale)
    void clearCanvas(unsigned char value = 0, unsigned char a = 255);

    // Rasterize helpers
    void setPixel(int x, int y, unsigned char v = 255, unsigned char a = 255);
    void drawDisc(int cx, int cy, int radius, unsigned char v = 255, unsigned char a = 255);
    void drawLine(int x0, int y0, int x1, int y1, int radius = 0, unsigned char v = 255, unsigned char a = 255);

    // Convert normalized [0,1] window coords (origin top-left)
    // into integer canvas pixel coords [0..kW-1],[0..kH-1]
    std::pair<int,int> toCanvas(float nx, float ny) const;

    std::vector<unsigned char> m_pixels; // 64x64x4 RGBA CPU buffer
    bool m_init = false;
    float m_time = 0.0f;
    std::mutex m_pixelsMutex; // guard m_pixels across update/render threads
};
