#include "strokeRecorder.h"

#include <algorithm>
#include <iostream>

StrokeRecorder* StrokeRecorder::instance() {
    static StrokeRecorder s;
    return &s;
}

glm::vec2 StrokeRecorder::normalizeToWindow(GLFWwindow* win, double x, double y) {
    int w = 1, h = 1;
    glfwGetWindowSize(win, &w, &h);
    if (w <= 0) w = 1;
    if (h <= 0) h = 1;
    // Origin top-left; y increases downward
    return glm::vec2(static_cast<float>(x / static_cast<double>(w)),
                     static_cast<float>(y / static_cast<double>(h)));
}

void StrokeRecorder::beginStroke(double now) {
    m_current = Stroke{};
    m_current.startTime = now;
    m_state = State::Drawing;
}

void StrokeRecorder::appendPoint(const glm::vec2& p) {
    // Optional: dedupe if not moved enough
    if (!m_current.points.empty()) {
        const glm::vec2 d = p - m_current.points.back();
        const float dist2 = d.x * d.x + d.y * d.y;
        // ~1px at 1000px dimension -> threshold ~1e-6 in normalized coords
        if (dist2 < 1e-7f) return;
    }
    m_current.points.emplace_back(p);
}

void StrokeRecorder::endStroke(double now) {
    m_current.endTime = now;
    if (!m_current.points.empty()) {
        m_completed.emplace_back(std::move(m_current));
    }
    m_current = Stroke{};
    m_state = State::Idle;
}

void StrokeRecorder::poll() {
    GLFWwindow* win = glfwGetCurrentContext();
    if (!win) return;

    const int btn = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);
    const bool pressed = (btn == GLFW_PRESS);
    const double now = glfwGetTime();

    if (pressed && !m_wasPressedLastFrame) {
        // Edge: press started
        std::lock_guard<std::mutex> lk(m_mutex);
        beginStroke(now);
    }

    if (pressed) {
        // Record point
        double cx = 0.0, cy = 0.0;
        glfwGetCursorPos(win, &cx, &cy);
        const glm::vec2 p = normalizeToWindow(win, cx, cy);
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_state == State::Drawing) appendPoint(p);
    }

    if (!pressed && m_wasPressedLastFrame) {
        // Edge: release
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_state == State::Drawing) endStroke(now);
    }

    m_wasPressedLastFrame = pressed;
}

bool StrokeRecorder::hasCompletedStroke() const {
    std::lock_guard<std::mutex> lk(m_mutex);
    return !m_completed.empty();
}

std::optional<StrokeRecorder::Stroke> StrokeRecorder::popCompletedStroke() {
    std::lock_guard<std::mutex> lk(m_mutex);
    if (m_completed.empty()) return std::nullopt;
    Stroke s = std::move(m_completed.front());
    m_completed.erase(m_completed.begin());
    return s;
}

std::vector<glm::vec2> StrokeRecorder::copyCurrentPoints() const {
    std::lock_guard<std::mutex> lk(m_mutex);
    return m_current.points;
}
