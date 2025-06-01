#pragma once

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>

enum class EventType {
    None = 0,
    Keyboard,
    Mouse,
    Window,
    Custom
};

struct Event {
    EventType type;
};

struct KeyboardEvent : public Event {
    int key;
    int action;
    KeyboardEvent(int k, int a) : key(k), action(a) {}
};

struct MouseEvent : public Event {
    int button;
    int action;
    int x;
    int y;
    MouseEvent(int b, int a, int posx, int posy) : button(b), action(a), x(posx), y(posy) {
        type = EventType::Mouse;
    }
};

struct WindowEvent : public Event {
    int action;
    WindowEvent(int a) : action(a) {
        type = EventType::Window;
    }
};

class EventHandler {
public:
    virtual ~EventHandler() = default;
    virtual void handleEvent(Event *event) = 0;
};

class EventDispatcher {
    std::queue<std::unique_ptr<Event>> m_eventQueue;
    std::unordered_map<EventType, std::vector<EventHandler *>> m_handlers;

public:
    void registerHandler(EventType type, EventHandler *handler) {
        m_handlers[type].push_back(handler);
    }

    void unregisterHandler(EventType type, EventHandler *handler) {
        auto &handlers = m_handlers[type];
        handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    }

    void dispatchEvent(std::unique_ptr<Event> event) {
        auto it = m_handlers.find(static_cast<EventType>(event->type));
        if (it != m_handlers.end()) {
            for (auto &handler : it->second) {
                handler->handleEvent(event.get());
            }
        }
        m_eventQueue.push(std::move(event));
    }

    std::unique_ptr<Event> pollEvent() {
        if (m_eventQueue.empty())
            return nullptr;
        auto event = std::move(m_eventQueue.front());
        m_eventQueue.pop();
        return event;
    }
};

class WindowEventHandler : public EventHandler {
public:
    void handleEvent(Event *event) override {
        if (event ->type == EventType::Window) {
            auto *winEvent = static_cast<WindowEvent *>(event);
            std::cout << "[WindowEventHandler] Received window event with action: " << winEvent->action << '\n';
        }
    }
};
