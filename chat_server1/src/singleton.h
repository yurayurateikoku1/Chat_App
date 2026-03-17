#pragma once

#include <memory>

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton<T> &) = delete;
    Singleton &operator=(const Singleton<T> &) = delete;
    static std::shared_ptr<T> instance_;

public:
    static std::shared_ptr<T> getInstance()
    {
        static std::once_flag flag;
        std::call_once(flag, [&]
                       { instance_ = std::shared_ptr<T>(new T); });
        return instance_;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;
