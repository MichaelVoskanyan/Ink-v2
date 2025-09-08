#pragma once

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

// StrokeRecorder
// - Milestone M1: Capture 2D points between LMB down/up.
// - Polls GLFW each frame; records a sequence of points for the active stroke.
// - Stores completed strokes for later consumption.
//
// Coordinates: window-normalized [0,1] with origin at top-left.
//   x = 0 at left edge, x = 1 at right edge
//   y = 0 at top edge,  y = 1 at bottom edge
class StrokeRecorder {
public:
    /// One continuous mouse-drag stroke.
    struct Stroke {
        /// Normalized [0,1] window coordinates sampled during the drag.
        std::vector<glm::vec2> points;
        /// Timestamp at press (glfwGetTime seconds).
        double startTime = 0.0;
        /// Timestamp at release (glfwGetTime seconds).
        double endTime = 0.0;
    };

    /// Recorder state machine: either idle or capturing a stroke.
    enum class State { Idle, Drawing };

    /// Access the singleton instance.
    static StrokeRecorder* instance();

    /// Sample mouse state and append points if LMB is held.
    /// Call once per frame from the main/render thread.
    void poll();

    /// True if one or more completed strokes are buffered.
    bool hasCompletedStroke() const;

    /// Remove and return the oldest completed stroke; std::nullopt if none.
    std::optional<Stroke> popCompletedStroke();

    /// Current capture state (idle/drawing).
    State state() const { return m_state; }
    /// Points collected for the in-progress stroke (empty when idle).
    const std::vector<glm::vec2>& currentPoints() const { return m_current.points; }

    /// Thread-safe snapshot of the current stroke's points.
    /// Copies the vector under a mutex so other threads can safely inspect.
    std::vector<glm::vec2> copyCurrentPoints() const;

private:
    StrokeRecorder() = default;

    // Helpers
    /// Convert raw cursor position (pixels) to normalized window coordinates.
    static glm::vec2 normalizeToWindow(GLFWwindow* win, double x, double y);
    /// Transition to Drawing and initialize a new Stroke.
    void beginStroke(double now);
    /// Append a point to the current stroke (with small de-duplication).
    void appendPoint(const glm::vec2& p);
    /// Finalize the stroke and queue it in the completed buffer.
    void endStroke(double now);

    mutable std::mutex m_mutex;
    State m_state = State::Idle;
    Stroke m_current;
    std::vector<Stroke> m_completed;
    bool m_wasPressedLastFrame = false;
};
