#ifndef INK_MEMORY
#define INK_MEMORY

#include <cstddef>

template <typename T>
class UniquePtr {
private:
    T *ptr;

public:
    explicit UniquePtr(T *p = nullptr) : ptr(p) {
    }

    UniquePtr(std::nullptr_t) : ptr(nullptr) {
    }

    ~UniquePtr() {
        delete ptr;
    }

    UniquePtr(UniquePtr &&other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    T *release() {
        T *tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T *p = nullptr) {
        delete ptr;
        ptr = p;
    }

    explicit operator bool() const noexcept { return ptr != nullptr; }

    bool operator==(std::nullptr_t) const noexcept { return ptr == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return ptr != nullptr; }

    template<typename T>
    bool operator==(const UniquePtr<T> &rhs) const noexcept {
        return ptr == rhs.ptr;
    }

    template<typename T>
    bool operator!=(const UniquePtr<T> &rhs) const noexcept {
        return ptr != rhs.ptr;
    }
};

template <typename T>
class SharedPtr {
    T *ptr;
    size_t *count;

    void release() {
        if (count) {
            if (--(*count) == 0) {
                delete ptr;
                delete count;
            }
        }
    }

public:
    template <typename U> friend class SharedPtr;

    explicit SharedPtr(T *p = nullptr) : ptr(p), count(p ? new size_t(1) : nullptr) {
    }

    SharedPtr(std::nullptr_t) : ptr(nullptr), count(nullptr) {
    }

    ~SharedPtr() {
        release();
    }

    SharedPtr(const SharedPtr &other) : ptr(other.ptr), count(other.count) {
        if (count)
            ++(*count);
    }

    SharedPtr &operator=(const SharedPtr &other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            if (count)
                ++(*count);
        }
        return *this;
    }

    SharedPtr(SharedPtr &&other) noexcept : ptr(other.ptr), count(other.count) {
        other.ptr = nullptr;
        other.count = nullptr;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

    template <typename U>
    SharedPtr(const SharedPtr<U> &other) noexcept : ptr(other.get()), count(other.count) {
        if (count) ++(*count);
    }

    template <typename U>
    SharedPtr &operator=(const SharedPtr<U> &other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            if (count)
                ++(*count);
        }
        return *this;
    }

    template <typename U>
    SharedPtr(SharedPtr<U> &&other) noexcept : ptr(other.get()), count(other.count) {
        other.ptr = nullptr;
        other.count = nullptr;
    }

    template <typename U>
    SharedPtr &operator=(SharedPtr<U> &&other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

    void reset(T *p = nullptr) {
        release();
        if (p) {
            ptr = p;
            count = new size_t(1);
        } else {
            ptr = nullptr;
            count = nullptr;
        }
    }

    T *get() const {
        return ptr;
    }
    T *operator->() const {
        return ptr;
    }
    T &operator*() const {
        return *ptr;
    }
    size_t useCount() const {
        return count ? *count : 0;
    }    

    explicit operator bool() const noexcept { return ptr != nullptr; }
    bool operator==(std::nullptr_t) const noexcept { return ptr == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return ptr != nullptr; }


    bool operator==(const SharedPtr<T>& rhs) const noexcept {
        return this->ptr == rhs.ptr;
    }

    bool operator!=(const SharedPtr<T>& rhs) const noexcept {
        return this->ptr != rhs.ptr;
    }
    
};


template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args &&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif