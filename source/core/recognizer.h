#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// Simple stub recognizer for M3 wiring.
// - submitStroke(points) triggers rasterize + mock classify immediately
// - popNewPrediction() returns a result once per submission
class Recognizer {
public:
    struct Prediction {
        std::string label;
        float confidence = 0.0f;
    };

    static Recognizer* instance();

    // Submit a completed stroke (normalized window coords; origin top-left).
    // Triggers immediate rasterization + mock inference.
    void submitStroke(const std::vector<glm::vec2>& points);

    // Non-blocking: returns a prediction only once per submission.
    std::optional<Prediction> popNewPrediction();

private:
    Recognizer() = default;

    // Rasterization helpers (64x64 RGBA internally but we just need luminance).
    static constexpr int kW = 64;
    static constexpr int kH = 64;
    std::vector<unsigned char> m_pixels; // grayscale 0..255

    static std::pair<int,int> toCanvas(float nx, float ny);
    void clear(unsigned char v = 0);
    void setPixel(int x, int y, unsigned char v = 255);
    void drawDisc(int cx, int cy, int r, unsigned char v = 255);
    void drawLine(int x0, int y0, int x1, int y1, int r = 1, unsigned char v = 255);

    // Very basic heuristic classifier for demo purposes
    Prediction classifyStub() const;

    bool m_hasNew = false;
    Prediction m_last;
};

