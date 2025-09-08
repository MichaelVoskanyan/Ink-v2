#include "recognizer.h"

#include <algorithm>
#include <cmath>

#include "core/drawing.h"

Recognizer* Recognizer::instance() {
    static Recognizer r;
    return &r;
}

static inline std::pair<int, int> toCanvasNoFlip(float nx, float ny, int w, int h) {
    nx = std::max(0.0f, std::min(1.0f, nx));
    ny = std::max(0.0f, std::min(1.0f, ny));
    int x = static_cast<int>(nx * (w - 1) + 0.5f);
    int y = static_cast<int>(ny * (h - 1) + 0.5f);
    return {x, y};
}

void Recognizer::submitStroke(const std::vector<glm::vec2>& points) {
    if (m_pixels.size() != static_cast<size_t>(kW * kH))
        m_pixels.assign(kW * kH, 0);
    else
        std::fill(m_pixels.begin(), m_pixels.end(), 0);

    if (points.empty()) {
        // Should not happen (endStroke guards), but be defensive
        m_last = {"none", 0.0f};
        m_hasNew = true;
        return;
    }

    Raster::ImageGray img{m_pixels.data(), kW, kH};

    auto [x0, y0] = toCanvasNoFlip(points.front().x, points.front().y, kW, kH);
    Raster::drawDisc(img, x0, y0, 1, 255);
    for (size_t i = 1; i < points.size(); ++i) {
        auto [x1, y1] = toCanvasNoFlip(points[i].x, points[i].y, kW, kH);
        Raster::drawLine(img, x0, y0, x1, y1, 1, 255);
        x0 = x1;
        y0 = y1;
    }

    // Stub classification
    // Count lit pixels and compute rough anisotropy
    double sum = 0.0;
    double cx = 0.0, cy = 0.0;
    for (int y = 0; y < kH; ++y) {
        for (int x = 0; x < kW; ++x) {
            unsigned char v = m_pixels[y * kW + x];
            double w = v / 255.0;
            sum += w;
            cx += w * x;
            cy += w * y;
        }
    }
    if (sum <= 1e-3) {
        m_last = {"none", 0.0f};
        m_hasNew = true;
        return;
    }
    cx /= sum; cy /= sum;

    double Ixx = 0, Iyy = 0;
    for (int y = 0; y < kH; ++y) {
        for (int x = 0; x < kW; ++x) {
            double w = m_pixels[y * kW + x] / 255.0;
            double dx = x - cx, dy = y - cy;
            Ixx += w * dy * dy; // variance in y
            Iyy += w * dx * dx; // variance in x
        }
    }
    double anisotropy = std::abs(Ixx - Iyy) / (Ixx + Iyy + 1e-6);

    std::string label = "blob";
    float conf = 0.5f;
    if (sum < 40.0) { label = "dot"; conf = 0.85f; }
    else if (anisotropy > 0.4) {
        // More variance in X (Iyy) => horizontal; more in Y (Ixx) => vertical
        label = (Iyy > Ixx) ? "horizontal" : "vertical";
        conf = static_cast<float>(0.6 + 0.4 * std::min(1.0, anisotropy));
    } else {
        label = "curve"; conf = 0.6f;
    }
    m_last = {label, conf};
    m_hasNew = true;
}

std::optional<Recognizer::Prediction> Recognizer::popNewPrediction() {
    if (!m_hasNew) return std::nullopt;
    m_hasNew = false;
    return m_last;
}
