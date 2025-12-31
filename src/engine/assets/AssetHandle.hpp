#pragma once

#include <memory>

template <typename T>
class AssetHandle {
public:
    AssetHandle() = default;
    explicit AssetHandle(std::shared_ptr<T> ptr)
        : m_ptr(std::move(ptr)) {}

    T* get() const { return m_ptr.get(); }
    std::shared_ptr<T> shared() const { return m_ptr; }
    explicit operator bool() const { return static_cast<bool>(m_ptr); }

private:
    std::shared_ptr<T> m_ptr;
};
